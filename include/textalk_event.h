/**
 * @file
 * @brief     Text communication library - callbacks.
 * @author    王文佑
 * @date      2016/11/11
 * @copyright ZLib Licence
 */
#ifndef _TEXTALK_EVENT_H_
#define _TEXTALK_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Data sender.
 * @details The callback function that will be called to send binary data.
 *
 * @param userarg An user defined argument.
 * @param data    The data to be sent.
 * @param size    Size of the input data.
 * @retval POSITIVE A positive value (including ZERO) indicates
 *         how many data in bytes have been sent out.
 * @retval NEGATIVE A negative value indicates error occurred.
 *
 * @remarks This function should work under non-blocking mode.
 */
typedef int(*textalk_sender_t)(void *userarg, const void *data, size_t size);

/**
 * @brief   Data receiver.
 * @details The callback function that will be called to receive binary data.
 *
 * @param userarg An user defined argument.
 * @param data    The buffer to receive data.
 * @param size    Size of the output data.
 * @retval POSITIVE A positive value (including ZERO) indicates
 *         how many data in bytes have been received.
 * @retval NEGATIVE A negative value indicates error occurred.
 *
 * @remarks This function should work under non-blocking mode.
 */
typedef int(*textalk_recver_t)(void *userarg, void *buf, size_t size);

/**
 * @brief   Event on control code sent.
 * @details The callback function that will be called when
 *          a control code be sent successfully.
 *
 * @param userarg An user defined argument.
 * @param code    The code that just be sent.
 */
typedef void(*textalk_on_send_ctrl_t)(void *userarg, char code);

/**
 * @brief   Event on control code received.
 * @details The callback function that will be called when
 *          a control code be received.
 *
 * @param userarg An user defined argument.
 * @param code    The code that just be received.
 */
typedef void(*textalk_on_recv_ctrl_t)(void *userarg, char code);

/**
 * @brief   Event on text sent.
 * @details The callback function that will be called when
 *          a text be sent successfully.
 *
 * @param userarg An user defined argument.
 * @param text    The text that just be sent.
 */
typedef void(*textalk_on_send_text_t)(void *userarg, const char *text);

/**
 * @brief   Event on text received.
 * @details The callback function that will be called when
 *          a text be received.
 *
 * @param userarg An user defined argument.
 * @param text    The text that just be received.
 */
typedef void(*textalk_on_recv_text_t)(void *userarg, const char *text);

/**
 * Event callbacks.
 */
typedef struct textalk_events_t
{
    void *userarg;  ///< A user defined argument that will be passed to callbacks.

    textalk_sender_t sender;    ///< Data sender, it is mandatory.
    textalk_recver_t recver;    ///< Data receiver, it is mandatory.

    textalk_on_send_ctrl_t on_send_ctrl;    ///< Event on control code sent,
                                            ///< it is optional and can be NULL to not use.
    textalk_on_recv_ctrl_t on_recv_ctrl;    ///< Event on control code received.
                                            ///< it is optional and can be NULL to not use.
    textalk_on_send_text_t on_send_text;    ///< Event on text sent.
                                            ///< it is optional and can be NULL to not use.
    textalk_on_recv_text_t on_recv_text;    ///< Event on text received.
                                            ///< it is optional and can be NULL to not use.

} textalk_events_t;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
