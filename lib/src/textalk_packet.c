#include <assert.h>
#include <string.h>
#include "parity.h"
#include "textalk_packet.h"

//------------------------------------------------------------------------------
bool textalk_packet_have_stx(const char *pkt, size_t size, const textalk_conf_t *conf)
{
    return ( size >= 1 ) && ( parity_ch_remove(pkt[0]) == conf->ctrl.stx );
}
//------------------------------------------------------------------------------
bool textalk_packet_have_etx(const char *pkt, size_t size, const textalk_conf_t *conf)
{
    if( conf->comm.have_lrc )
        return ( size >= 3 ) && ( parity_ch_remove(pkt[size-2]) == conf->ctrl.etx );
    else
        return ( size >= 2 ) && ( parity_ch_remove(pkt[size-1]) == conf->ctrl.etx );
}
//------------------------------------------------------------------------------
bool textalk_packet_have_etb(const char *pkt, size_t size, const textalk_conf_t *conf)
{
    if( conf->comm.have_lrc )
        return ( size >= 3 ) && ( parity_ch_remove(pkt[size-2]) == conf->ctrl.etb );
    else
        return ( size >= 2 ) && ( parity_ch_remove(pkt[size-1]) == conf->ctrl.etb );
}
//------------------------------------------------------------------------------
static
char pkt_calc_lrc(const char *pkt, size_t size)
{
    assert( pkt && size >= 2 );

    // Skip the first character (STX).
    ++pkt;
    --size;

    char lrc = 0;
    while( --size )  // Skip the last character (LRC).
        lrc ^= *pkt++;

    return lrc;
}
//------------------------------------------------------------------------------
static
char pkt_get_lrc(const char *pkt, size_t size)
{
    assert( pkt && size );
    return pkt[size-1];
}
//------------------------------------------------------------------------------
static
void pkt_set_lrc(char *pkt, size_t size, char lrc)
{
    assert( pkt && size );
    pkt[size-1] = lrc;
}
//------------------------------------------------------------------------------
size_t textalk_packet_build(char                 *buf,
                            size_t                bufsz,
                            const char           *text,
                            const textalk_conf_t *conf,
                            bool                  havemore)
{
    assert( buf && text );

    char   *pkt     = buf;
    size_t  textlen = strlen(text);
    size_t  pktsz   = (1/*STX*/) + textlen + (1/*ETX*/) + ( conf->comm.have_lrc ? 1 : 0 );
    if( bufsz < pktsz ) return 0;

    pkt[0] = conf->ctrl.stx;
    memcpy(&pkt[1], text, textlen);
    pkt[textlen+1] = havemore ? conf->ctrl.etb : conf->ctrl.etx;

    parity_arr_add(pkt, pktsz, conf->comm.parity);
    if( conf->comm.have_lrc )
        pkt_set_lrc(pkt, pktsz, pkt_calc_lrc(pkt, pktsz));

    return pktsz;
}
//------------------------------------------------------------------------------
bool textalk_packet_get_text(char                 *buf,
                             size_t                bufsz,
                             const char           *pkt,
                             size_t                pktsz,
                             const textalk_conf_t *conf)
{
    if( !textalk_packet_check_integrity(pkt, pktsz, conf) )
        return false;

    size_t textlen = pktsz - ( conf->comm.have_lrc ? 3 : 2 );
    if( bufsz < textlen + 1 ) return false;

    memcpy(buf, pkt+1, textlen);
    buf[textlen] = 0;

    parity_arr_remove(buf, textlen, conf->comm.parity);

    return true;
}
//------------------------------------------------------------------------------
bool textalk_packet_check_integrity(const char *pkt, size_t size, const textalk_conf_t *conf)
{
    return textalk_packet_have_stx(pkt, size, conf) &&
           ( textalk_packet_have_etx(pkt, size, conf) ||
             textalk_packet_have_etb(pkt, size, conf) );
}
//------------------------------------------------------------------------------
bool textalk_packet_check_parity(const char *pkt, size_t size, const textalk_conf_t *conf)
{
    if( !size ) return true;

    if( !conf->comm.have_lrc ) --size;
    return parity_arr_check(pkt, size, conf->comm.parity);
}
//------------------------------------------------------------------------------
bool textalk_packet_check_lrc(const char *pkt, size_t size, const textalk_conf_t *conf)
{
    return !conf->comm.have_lrc ||
           pkt_calc_lrc(pkt, size) == pkt_get_lrc(pkt, size);
}
//------------------------------------------------------------------------------
bool textalk_packet_check_all(const char *pkt, size_t size, const textalk_conf_t *conf)
{
    return textalk_packet_check_integrity(pkt, size, conf) &&
           textalk_packet_check_parity(pkt, size, conf) &&
           textalk_packet_check_lrc(pkt, size, conf);
}
//------------------------------------------------------------------------------
