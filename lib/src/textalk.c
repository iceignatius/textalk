#include <assert.h>
#include <ctype.h>
#include <gen/jmpbk.h>
#include <gen/bufstm.h>
#include <gen/timectr.h>
#include "parity.h"
#include "textalk_packet.h"
#include "textalk.h"

//------------------------------------------------------------------------------
static
void on_send_ctrl_default(void *userarg, char code)
{
    // Nothing to do.
}
//------------------------------------------------------------------------------
static
void on_recv_ctrl_default(void *userarg, char code)
{
    // Nothing to do.
}
//------------------------------------------------------------------------------
static
void on_send_text_default(void *userarg, const char *text)
{
    // Nothing to do.
}
//------------------------------------------------------------------------------
static
void on_recv_text_default(void *userarg, const char *text)
{
    // Nothing to do.
}
//------------------------------------------------------------------------------
void textalk_init(textalk_t              *self,
                  const textalk_conf_t   *conf,
                  const textalk_events_t *events)
{
    /**
     * @memberof textalk_t
     * @brief Constructor.
     *
     * @param self         Object instance.
     * @param conf         Communication configuration,
     *                     and can be NULL to use default values.
     * @param events       A set of event callbacks.
     */
    self->conf = *conf;

    assert( events->sender && events->recver );
    self->events = *events;

    if( !self->events.on_send_ctrl ) self->events.on_send_ctrl = on_send_ctrl_default;
    if( !self->events.on_recv_ctrl ) self->events.on_recv_ctrl = on_recv_ctrl_default;
    if( !self->events.on_send_text ) self->events.on_send_text = on_send_text_default;
    if( !self->events.on_recv_text ) self->events.on_recv_text = on_recv_text_default;
}
//------------------------------------------------------------------------------
void textalk_deinit(textalk_t *self)
{
    /**
     * @memberof textalk_t
     * @brief Destructor.
     */

    // Nothing to do.
}
//------------------------------------------------------------------------------
int textalk_send_ctrl(textalk_t *self, char code)
{
    /**
     * @memberof textalk_t
     * @brief Send a control code.
     *
     * @param self Object instance.
     * @param code The character to be sent.
     * @return One of error codes defined in ::textalk_errcode_t.
     */
    char data = parity_ch_add(code, self->conf.comm.parity);

    int sendsz = 0;
    timectr_t timer = timectr_init_inline(self->conf.comm.timeout.send);
    while( !sendsz && !timectr_is_expired(&timer) )
    {
        sendsz = self->events.sender(self->events.userarg, &data, sizeof(data));
        if( !sendsz ) systime_sleep_awhile();
    }

    if( sendsz < 0 ) return TEXTALK_ERR_STREAM_FAIL;
    if( sendsz == 0 ) return TEXTALK_ERR_TIMEOUT;

    self->events.on_send_ctrl(self->events.userarg, code);
    return TEXTALK_ERR_SUCCESS;
}
//------------------------------------------------------------------------------
int textalk_wait_ctrl(textalk_t *self, char target, char *result)
{
    /**
     * @memberof textalk_t
     * @brief Waiting for a control code.
     *
     * @param self   Object instance.
     * @param target The target character to wait;
     *               and this parameter can be ZERO to waiting any control codes.
     * @param result Return the final code that be received;
     *               and can be NULL to not report.
     * @return One of error codes defined in ::textalk_errcode_t.
     */
    char ch = 0;

    int recvsz = 0;
    timectr_t timer = timectr_init_inline(self->conf.comm.timeout.echo);
    while( !ch && recvsz >= 0 && !timectr_is_expired(&timer) )
    {
        recvsz = self->events.recver(self->events.userarg, &ch, sizeof(ch));
        if( recvsz > 0 )
        {
            ch = parity_ch_remove(ch);
            if( !iscntrl(ch) || ( target && ch != target ) )
                ch = 0;
        }
        else if( recvsz == 0 )
        {
            systime_sleep_awhile();
        }
    }

    if( recvsz < 0 ) return TEXTALK_ERR_STREAM_FAIL;
    if( recvsz == 0 ) return TEXTALK_ERR_TIMEOUT;
    if( !ch ) return TEXTALK_ERR_TIMEOUT;

    if( result ) *result = ch;
    self->events.on_recv_ctrl(self->events.userarg, ch);
    return TEXTALK_ERR_SUCCESS;
}
//------------------------------------------------------------------------------
static
int textalk_send_packet(textalk_t *self, const char *pkt, size_t size)
{
    timectr_t timer = timectr_init_inline(self->conf.comm.timeout.send);
    while( size && !timectr_is_expired(&timer) )
    {
        int sendsz = self->events.sender(self->events.userarg, pkt, size);
        if( sendsz < 0 || size < sendsz ) return TEXTALK_ERR_STREAM_FAIL;

        if( sendsz )
        {
            pkt  += sendsz;
            size -= sendsz;
        }
        else
        {
            systime_sleep_awhile();
        }
    }

    return size ? TEXTALK_ERR_TIMEOUT : TEXTALK_ERR_SUCCESS;
}
//------------------------------------------------------------------------------
static
int textalk_send_packet_and_wait_echo(textalk_t  *self,
                                      const char *text,
                                      const char *pkt,
                                      size_t      size)
{
    int errcode;

    if(( errcode = textalk_send_packet(self, pkt, size) )) return errcode;
    self->events.on_send_text(self->events.userarg, text);

    char echo;
    if(( errcode = textalk_wait_ctrl(self, 0, &echo) )) return errcode;

    if( echo == self->conf.ctrl.eot ) return TEXTALK_ERR_TERMINATED;
    if( echo != self->conf.ctrl.ack ) return TEXTALK_ERR_BAD_EXCHANGE;

    return TEXTALK_ERR_SUCCESS;
}
//------------------------------------------------------------------------------
int textalk_send_text(textalk_t *self, const char *text, bool havemore)
{
    /**
     * @memberof textalk_t
     * @brief Send out a text.
     *
     * @param self     Object instance.
     * @param text     The text to be sent.
     * @param havemore Set TRUE to notify the remote that
     *                 there have more text to send;
     *                 and set FALSE to notify that
     *                 this is the last text for this exchange round.
     * @return One of error codes defined in ::textalk_errcode_t.
     */
    if( !text ) return TEXTALK_ERR_INVALID_ARG;

    char pkt[TEXTALK_PKT_MAX_SIZE];
    size_t pktsz = textalk_packet_build(pkt, sizeof(pkt), text, &self->conf, havemore);
    if( !pktsz ) return TEXTALK_ERR_BUF_NOT_ENOUGH;

    int      errcode = TEXTALK_ERR_GENERAL;
    unsigned trycnt  = self->conf.comm.retry_max + 1;
    while( errcode &&
           errcode != TEXTALK_ERR_STREAM_FAIL &&
           errcode != TEXTALK_ERR_TERMINATED &&
           trycnt-- )
    {
        errcode = textalk_send_packet_and_wait_echo(self, text, pkt, pktsz);
    }

    return errcode;
}
//------------------------------------------------------------------------------
static
int recv_for_stx_only(textalk_t *self, bufostm_t *outstm, timectr_t *timer)
{
    char stx = parity_ch_add(self->conf.ctrl.stx, self->conf.comm.parity);

    char ch;
    while( !timectr_is_expired(timer) )
    {
        int recvsz = self->events.recver(self->events.userarg, &ch, sizeof(ch));
        if( recvsz < 0 )
        {
            return TEXTALK_ERR_STREAM_FAIL;
        }
        else if( recvsz == 0 )
        {
            systime_sleep_awhile();
        }
        else if( ch == stx )
        {
            return bufostm_write(outstm, &ch, sizeof(ch)) ?
                   TEXTALK_ERR_SUCCESS : TEXTALK_ERR_BUF_NOT_ENOUGH;
        }
    }

    return TEXTALK_ERR_TIMEOUT;
}
//------------------------------------------------------------------------------
static
int recv_all_until_etx_or_etb_reached(textalk_t *self, bufostm_t *outstm, timectr_t *timer)
{
    char etx = parity_ch_add(self->conf.ctrl.etx, self->conf.comm.parity);
    char etb = parity_ch_add(self->conf.ctrl.etb, self->conf.comm.parity);

    char ch;
    while( ch != etx && ch != etb && !timectr_is_expired(timer) )
    {
        int recvsz = self->events.recver(self->events.userarg, &ch, sizeof(ch));
        if( recvsz < 0 )
        {
            return TEXTALK_ERR_STREAM_FAIL;
        }
        else if( recvsz == 0 )
        {
            systime_sleep_awhile();
        }
        else
        {
            if( !bufostm_write(outstm, &ch, sizeof(ch)) )
                return TEXTALK_ERR_BUF_NOT_ENOUGH;
        }
    }

    return ( ch == etx || ch == etb )?( TEXTALK_ERR_SUCCESS ):( TEXTALK_ERR_TIMEOUT );
}
//------------------------------------------------------------------------------
static
int recv_one_byte(textalk_t *self, bufostm_t *outstm, timectr_t *timer)
{
    char ch;
    while( !timectr_is_expired(timer) )
    {
        int recvsz = self->events.recver(self->events.userarg, &ch, sizeof(ch));
        if( recvsz < 0 )
        {
            return TEXTALK_ERR_STREAM_FAIL;
        }
        else if( recvsz == 0 )
        {
            systime_sleep_awhile();
        }
        else
        {
            return bufostm_write(outstm, &ch, sizeof(ch)) ?
                   TEXTALK_ERR_SUCCESS : TEXTALK_ERR_BUF_NOT_ENOUGH;
        }
    }

    return TEXTALK_ERR_TIMEOUT;
}
//------------------------------------------------------------------------------
static
int textalk_recv_packet(textalk_t *self, char *buf, size_t bufsz, size_t *pktsz)
{
    timectr_t timer = timectr_init_inline(self->conf.comm.timeout.resp);

    bufostm_t stream;
    bufostm_init(&stream, buf, bufsz);

    int errcode;

    if(( errcode = recv_for_stx_only(self, &stream, &timer) ))
        return errcode;

    if(( errcode = recv_all_until_etx_or_etb_reached(self, &stream, &timer) ))
        return errcode;

    if( self->conf.comm.have_lrc )
    {
        if(( errcode = recv_one_byte(self, &stream, &timer) ))
            return errcode;
    }

    *pktsz = bufostm_get_datasize(&stream);

    return TEXTALK_ERR_SUCCESS;
}
//------------------------------------------------------------------------------
static
int textalk_wait_text_without_retry(textalk_t *self, char *buf, size_t bufsize, bool *havemore)
{
    int res;
    JMPBK_BEGIN
    {
        int errcode;

        char   pkt[TEXTALK_PKT_MAX_SIZE];
        size_t pktsz = 0;
        if(( errcode = textalk_recv_packet(self, pkt, sizeof(pkt), &pktsz) ))
            JMPBK_THROW(errcode);

        if( !textalk_packet_check_all(pkt, pktsz, &self->conf) )
            JMPBK_THROW(TEXTALK_ERR_BAD_EXCHANGE);

        *havemore = textalk_packet_have_etb(pkt, pktsz, &self->conf);

        if( !textalk_packet_get_text(buf, bufsize, pkt, pktsz, &self->conf) )
            JMPBK_THROW(TEXTALK_ERR_BUF_NOT_ENOUGH);

        self->events.on_recv_text(self->events.userarg, buf);
    }
    JMPBK_FINAL
    {
        res = JMPBK_ERRCODE;
        char echo = res ? self->conf.ctrl.nak : self->conf.ctrl.ack;
        textalk_send_ctrl(self, echo);
    }
    JMPBK_END

    return res;
}
//------------------------------------------------------------------------------
int textalk_wait_text(textalk_t *self, char *buf, size_t bufsize)
{
    /**
     * @memberof textalk_t
     * @brief Receive text response.
     *
     * @param self    Object instance.
     * @param buf     A buffer to receive the text data.
     *                And a null-terminator will be appended at the end of text
     *                if the buffer is large enough.
     * @param bufsize Size of the output buffer.
     * @return One of error codes defined in ::textalk_errcode_t.
     *
     * @remarks This function may returns ::TEXTALK_ERR_HAVE_MORE,
     *          and it does not means failure.
     *          It actually means that the text be received successfully,
     *          but there are more text needs to receive.
     */
    if( !buf || !bufsize ) return TEXTALK_ERR_INVALID_ARG;

    bool havemore = false;

    int      errcode = TEXTALK_ERR_GENERAL;
    unsigned trycnt  = self->conf.comm.retry_max + 1;
    while( errcode &&
           errcode != TEXTALK_ERR_STREAM_FAIL &&
           errcode != TEXTALK_ERR_TERMINATED &&
           trycnt-- )
    {
        errcode = textalk_wait_text_without_retry(self, buf, bufsize, &havemore);
    }

    return ( errcode )?( errcode ):
           ( havemore ? TEXTALK_ERR_HAVE_MORE : TEXTALK_ERR_SUCCESS );
}
//------------------------------------------------------------------------------
