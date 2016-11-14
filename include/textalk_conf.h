/**
 * @file
 * @brief     Text communication library - configuration.
 * @author    王文佑
 * @date      2016/09/30
 * @copyright ZLib Licence
 */
#ifndef _TEXTALK_CONF_H_
#define _TEXTALK_CONF_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Text talk character configuration.
 * @details Characters to be used as what it named.
 */
typedef struct textalk_conf_ctrl_t
{
    char stx;   ///< STX.
    char etx;   ///< ETX.
    char etb;   ///< ETB.
    char enq;   ///< ENQ.
    char ack;   ///< ACK.
    char nak;   ///< NAK.
    char eot;   ///< EOT.
} textalk_conf_ctrl_t;

/**
 * Parity configuration.
 */
enum textalk_conf_parity_t
{
    TEXTALK_PARITY_NONE = 0,    ///< No parity.
    TEXTALK_PARITY_ODD  = 1,    ///< Odd parity.
    TEXTALK_PARITY_EVEN = 2,    ///< Even parity.
};

/**
 * Text talk time-out configuration.
 */
typedef struct textalk_conf_timeout_t
{
    unsigned send;  ///< Time-out when sending data in milliseconds.
    unsigned echo;  ///< Time-out when waiting response code in milliseconds.
    unsigned resp;  ///< Time-out when waiting text in milliseconds.
} textalk_conf_timeout_t;

/**
 * Text talk communication configuration.
 */
typedef struct textalk_conf_comm_t
{
    int      parity;        ///< Parity, see ::textalk_conf_parity_t for more information.
    bool     have_lrc;      ///< Does packet have LRC or not.
    unsigned retry_max;     ///< The maximum count to retry text send or receive.

    textalk_conf_timeout_t timeout;     ///< Time-out configuration.

} textalk_conf_comm_t;

/**
 * Text talk configuration.
 */
typedef struct textalk_conf_t
{
    textalk_conf_ctrl_t ctrl;   ///< Control characters.
    textalk_conf_comm_t comm;   ///< Communication.
} textalk_conf_t;

const textalk_conf_t* textalk_conf_get_defaults(void);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
