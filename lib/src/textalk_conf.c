#include <gen/ascii.h>
#include "textalk_conf.h"

static const textalk_conf_t conf_default =
{
    .ctrl =
    {
        .stx    = ASCII_STX,
        .etx    = ASCII_ETX,
        .etb    = ASCII_ETB,
        .enq    = ASCII_ENQ,
        .ack    = ASCII_ACK,
        .nak    = ASCII_NAK,
        .eot    = ASCII_EOT,
    },
    .comm =
    {
        .parity     = TEXTALK_PARITY_NONE,
        .have_lrc   = true,
        .retry_max  = 3,
        .timeout =
        {
            .send   = 500,
            .echo   = 500,
            .resp   = 3000,
        },
    },
};
//------------------------------------------------------------------------------
const textalk_conf_t* textalk_conf_get_defaults(void)
{
    /**
     * Get default configuration.
     */
    return &conf_default;
}
//------------------------------------------------------------------------------
