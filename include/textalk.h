/**
 * @file
 * @brief     Text communication library.
 * @author    王文佑
 * @date      2016/09/30
 * @copyright ZLib Licence
 */
#ifndef _TEXTALK_H_
#define _TEXTALK_H_

#include <stddef.h>

#ifdef __cplusplus
#include <string>
#endif

#include "textalk_conf.h"
#include "textalk_event.h"
#include "textalk_errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEXTALK_PKT_MAX_SIZE 1024  // The maximum size of the packet buffer.

/**
 * @class textalk_t
 * @brief Text talk class.
 */
typedef struct textalk_t
{
    textalk_conf_t   conf;
    textalk_events_t events;
} textalk_t;

void textalk_init(textalk_t              *self,
                  const textalk_conf_t   *conf,
                  const textalk_events_t *events);
void textalk_deinit(textalk_t *self);

int textalk_send_ctrl(textalk_t *self, char code);
int textalk_wait_ctrl(textalk_t *self, char target, char *result);
int textalk_send_text(textalk_t *self, const char *text, bool havemore);
int textalk_wait_text(textalk_t *self, char *buf, size_t bufsize);

#ifdef __cplusplus
}  // extern "C"
#endif

#ifdef __cplusplus

/**
 * @brief C++ wrapper of @ref textalk_t
 */
class TTextalk : protected textalk_t
{
public:

    TTextalk(const textalk_conf_t &conf)
    {
        textalk_events_t events =
        {
            this,
            (int(*)(void*,const void*,size_t)) SenderCallback,
            (int(*)(void*,void*,size_t))       ReceiverCallback,
            (void(*)(void*,char))        OnSendControlCallback,
            (void(*)(void*,char))        OnReceiveControlCallback,
            (void(*)(void*,const char*)) OnSendTextCallback,
            (void(*)(void*,const char*)) OnReceiveTextCallback
        };

        textalk_init(this,
                     &conf,
                     &events);
    }

    ~TTextalk()
    {
        textalk_deinit(this);
    }

private:
    TTextalk(const TTextalk &src);              // Not allowed to use!
    TTextalk& operator=(const TTextalk &src);   // Not allowed to use!

private:

    static int SenderCallback(TTextalk *self, const void *data, size_t size)
    {
        return self->Sender(data, size);
    }

    static int ReceiverCallback(TTextalk *self, void *buf, size_t size)
    {
        return self->Receiver(buf, size);
    }

    static void OnSendControlCallback(TTextalk *self, char code)
    {
        return self->OnSendControl(code);
    }

    static void OnReceiveControlCallback(TTextalk *self, char code)
    {
        return self->OnReceiveControl(code);
    }

    static void OnSendTextCallback(TTextalk *self, const char *text)
    {
        return self->OnSendText(text);
    }

    static void OnReceiveTextCallback(TTextalk *self, const char *text)
    {
        return self->OnReceiveText(text);
    }

protected:

    /// Binary data sender, @see textalk_sender.
    virtual int Sender(const void *data, size_t size) = 0;

    /// Binary data receiver, @see textalk_recver.
    virtual int Receiver(void *buf, size_t size) = 0;

    /// Event on control code sent, @see textalk_on_send_ctrl_t.
    virtual void OnSendControl(char code) {}

    /// Event on control code received, @see textalk_on_recv_ctrl_t.
    virtual void OnReceiveControl(char code) {}

    /// Event on control text sent, @see textalk_on_send_text_t.
    virtual void OnSendText(const std::string &text) {}

    /// Event on control text received, @see textalk_on_recv_text_t.
    virtual void OnReceiveText(const std::string &text) {}

public:

    int SendCtrl(char code)
    {
        /// @see textalk_t::textalk_send_ctrl
        return textalk_send_ctrl(this, code);
    }

    int WaitCtrl(char target, char &result)
    {
        /// @see textalk_t::textalk_wait_ctrl
        return textalk_wait_ctrl(this, target, &result);
    }

    int SendText(const std::string &text, bool havemore)
    {
        /// @see textalk_t::textalk_send_text
        return textalk_send_text(this, text.c_str(), havemore);
    }

    int WaitText(std::string &text)
    {
        /// @see textalk_t::textalk_wait_text

        static const size_t bufsize =1024;
        char buf[bufsize] = {0};
        int res = textalk_wait_text(this, buf, sizeof(buf)-1);

        text = buf;
        return res;
    }

};

#endif   // __cplusplus

#endif
