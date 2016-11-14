/**
 * @file
 * @brief     Text communication library - error codes definition.
 * @author    王文佑
 * @date      2016/09/30
 * @copyright ZLib Licence
 */
#ifndef _TEXTALK_ERRCODE_H_
#define _TEXTALK_ERRCODE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error code definition.
 */
enum textalk_errcode_t
{
    TEXTALK_ERR_SUCCESS     = 0,    ///< Success.
    TEXTALK_ERR_HAVE_MORE   = 1,    ///< Get text successfully, but have more to receive.

    TEXTALK_ERR_INVALID_ARG,        ///< Invalid argument!
    TEXTALK_ERR_BUF_NOT_ENOUGH,     ///< Buffer not long enough!
    TEXTALK_ERR_STREAM_FAIL,        ///< Error occurred in sender or receiver callback!
    TEXTALK_ERR_BAD_EXCHANGE,       ///< Communication be aborted by the bad packet data or echo!
    TEXTALK_ERR_TERMINATED,         ///< Communication be terminated by remote!
    TEXTALK_ERR_TIMEOUT,            ///< Time-out!

    TEXTALK_ERR_GENERAL     = -1,
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
