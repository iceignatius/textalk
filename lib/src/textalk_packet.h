/*
 * Text packet encoder and decoder.
 */
#ifndef _TEXTALK_PACKET_H_
#define _TEXTALK_PACKET_H_

#include <stddef.h>
#include <stdbool.h>
#include "textalk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

bool textalk_packet_have_stx(const char *pkt, size_t size, const textalk_conf_t *conf);
bool textalk_packet_have_etx(const char *pkt, size_t size, const textalk_conf_t *conf);
bool textalk_packet_have_etb(const char *pkt, size_t size, const textalk_conf_t *conf);

size_t textalk_packet_build(char                 *buf,
                            size_t                bufsz,
                            const char           *text,
                            const textalk_conf_t *conf,
                            bool                  havemore);
bool textalk_packet_get_text(char                 *buf,
                             size_t                bufsz,
                             const char           *pkt,
                             size_t                pktsz,
                             const textalk_conf_t *conf);

bool textalk_packet_check_integrity(const char *pkt, size_t size, const textalk_conf_t *conf);
bool textalk_packet_check_parity(const char *pkt, size_t size, const textalk_conf_t *conf);
bool textalk_packet_check_lrc(const char *pkt, size_t size, const textalk_conf_t *conf);
bool textalk_packet_check_all(const char *pkt, size_t size, const textalk_conf_t *conf);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
