/* Copyright (c) 2001, Matej Pfajfar.
 * Copyright (c) 2001-2004, Roger Dingledine.
 * Copyright (c) 2004-2006, Roger Dingledine, Nick Mathewson.
 * Copyright (c) 2007-2017, The Tor Project, Inc. */
/* See LICENSE for licensing information */

/**
 * \file crypto_digest.h
 *
 * \brief Headers for crypto_digest.c
 **/

#ifndef TOR_CRYPTO_DIGEST_H
#define TOR_CRYPTO_DIGEST_H

#include <stdio.h>
#include "crypto_rsa.h"

/** Length of the output of our message digest. */
#define DIGEST_LEN 20
/** Length of the output of our second (improved) message digests.  (For now
 * this is just sha256, but it could be any other 256-bit digest.) */
#define DIGEST256_LEN 32
/** Length of the output of our 64-bit optimized message digests (SHA512). */
#define DIGEST512_LEN 64

/** Length of a sha1 message digest when encoded in base32 with trailing =
 * signs removed. */
#define BASE32_DIGEST_LEN 32
/** Length of a sha1 message digest when encoded in base64 with trailing =
 * signs removed. */
#define BASE64_DIGEST_LEN 27
/** Length of a sha256 message digest when encoded in base64 with trailing =
 * signs removed. */
#define BASE64_DIGEST256_LEN 43
/** Length of a sha512 message digest when encoded in base64 with trailing =
 * signs removed. */
#define BASE64_DIGEST512_LEN 86

/** Length of hex encoding of SHA1 digest, not including final NUL. */
#define HEX_DIGEST_LEN 40
/** Length of hex encoding of SHA256 digest, not including final NUL. */
#define HEX_DIGEST256_LEN 64
/** Length of hex encoding of SHA512 digest, not including final NUL. */
#define HEX_DIGEST512_LEN 128

typedef enum {
  DIGEST_SHA1 = 0,
  DIGEST_SHA256 = 1,
  DIGEST_SHA512 = 2,
  DIGEST_SHA3_256 = 3,
  DIGEST_SHA3_512 = 4,
} digest_algorithm_t;
#define  N_DIGEST_ALGORITHMS (DIGEST_SHA3_512+1)
#define  N_COMMON_DIGEST_ALGORITHMS (DIGEST_SHA256+1)

/** A set of all the digests we commonly compute, taken on a single
 * string.  Any digests that are shorter than 512 bits are right-padded
 * with 0 bits.
 *
 * Note that this representation wastes 44 bytes for the SHA1 case, so
 * don't use it for anything where we need to allocate a whole bunch at
 * once.
 **/
typedef struct {
  char d[N_COMMON_DIGEST_ALGORITHMS][DIGEST256_LEN];
} common_digests_t;

typedef struct crypto_digest_t crypto_digest_t;
typedef struct crypto_xof_t crypto_xof_t;

/* public key crypto digest */
MOCK_DECL(int, crypto_pk_public_checksig_digest,(crypto_pk_t *env,
                                         const char *data, size_t datalen,
                                         const char *sig, size_t siglen));
int crypto_pk_private_sign_digest(crypto_pk_t *env, char *to, size_t tolen,
                                  const char *from, size_t fromlen);
int crypto_pk_get_digest(const crypto_pk_t *pk, char *digest_out);
int crypto_pk_get_common_digests(crypto_pk_t *pk,
                                 common_digests_t *digests_out);

/* SHA-1 and other digests */
int crypto_digest(char *digest, const char *m, size_t len);
int crypto_digest256(char *digest, const char *m, size_t len,
                     digest_algorithm_t algorithm);
int crypto_digest512(char *digest, const char *m, size_t len,
                     digest_algorithm_t algorithm);
int crypto_common_digests(common_digests_t *ds_out, const char *m, size_t len);
void crypto_digest_smartlist_prefix(char *digest_out, size_t len_out,
                                    const char *prepend,
                                    const struct smartlist_t *lst,
                                    const char *append,
                                    digest_algorithm_t alg);
void crypto_digest_smartlist(char *digest_out, size_t len_out,
                             const struct smartlist_t *lst, const char *append,
                             digest_algorithm_t alg);
const char *crypto_digest_algorithm_get_name(digest_algorithm_t alg);
size_t crypto_digest_algorithm_get_length(digest_algorithm_t alg);
int crypto_digest_algorithm_parse_name(const char *name);
crypto_digest_t *crypto_digest_new(void);
crypto_digest_t *crypto_digest256_new(digest_algorithm_t algorithm);
crypto_digest_t *crypto_digest512_new(digest_algorithm_t algorithm);
void crypto_digest_free_(crypto_digest_t *digest);
#define crypto_digest_free(d) \
  FREE_AND_NULL(crypto_digest_t, crypto_digest_free_, (d))
void crypto_digest_add_bytes(crypto_digest_t *digest, const char *data,
                             size_t len);
void crypto_digest_get_digest(crypto_digest_t *digest,
                              char *out, size_t out_len);
crypto_digest_t *crypto_digest_dup(const crypto_digest_t *digest);
void crypto_digest_assign(crypto_digest_t *into,
                          const crypto_digest_t *from);
void crypto_hmac_sha256(char *hmac_out,
                        const char *key, size_t key_len,
                        const char *msg, size_t msg_len);
void crypto_mac_sha3_256(uint8_t *mac_out, size_t len_out,
                         const uint8_t *key, size_t key_len,
                         const uint8_t *msg, size_t msg_len);

/* xof functions*/
crypto_xof_t *crypto_xof_new(void);
void crypto_xof_add_bytes(crypto_xof_t *xof, const uint8_t *data, size_t len);
void crypto_xof_squeeze_bytes(crypto_xof_t *xof, uint8_t *out, size_t len);
void crypto_xof_free_(crypto_xof_t *xof);
#define crypto_xof_free(xof) \
  FREE_AND_NULL(crypto_xof_t, crypto_xof_free_, (xof))

#ifdef TOR_UNIT_TESTS
digest_algorithm_t crypto_digest_get_algorithm(crypto_digest_t *digest);
#endif

#endif /* !defined(TOR_CRYPTO_DIGEST_H) */

