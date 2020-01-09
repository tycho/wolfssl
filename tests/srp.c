/* srp.c SRP unit tests
 *
 * Copyright (C) 2006-2020 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#include <tests/unit.h>
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/srp.h>

#if defined(WOLFCRYPT_HAVE_SRP) && defined(WOLFSSL_SHA512)

static byte username[] = "user";
static word32 usernameSz = 4;

static byte password[] = "password";
static word32 passwordSz = 8;

static byte N[] = {
    0xD4, 0xC7, 0xF8, 0xA2, 0xB3, 0x2C, 0x11, 0xB8, 0xFB, 0xA9, 0x58, 0x1E,
    0xC4, 0xBA, 0x4F, 0x1B, 0x04, 0x21, 0x56, 0x42, 0xEF, 0x73, 0x55, 0xE3,
    0x7C, 0x0F, 0xC0, 0x44, 0x3E, 0xF7, 0x56, 0xEA, 0x2C, 0x6B, 0x8E, 0xEB,
    0x75, 0x5A, 0x1C, 0x72, 0x30, 0x27, 0x66, 0x3C, 0xAA, 0x26, 0x5E, 0xF7,
    0x85, 0xB8, 0xFF, 0x6A, 0x9B, 0x35, 0x22, 0x7A, 0x52, 0xD8, 0x66, 0x33,
    0xDB, 0xDF, 0xCA, 0x43
};

static byte g[] = {
    0x02
};

static byte salt[] = {
    0x80, 0x66, 0x61, 0x5B, 0x7D, 0x33, 0xA2, 0x2E, 0x79, 0x18
};

static byte verifier[] = {
    0x24, 0x5F, 0xA5, 0x1B, 0x2A, 0x28, 0xF8, 0xFF, 0xE2, 0xA0, 0xF8, 0x61,
    0x7B, 0x0F, 0x3C, 0x05, 0xD6, 0x4A, 0x55, 0xDF, 0x74, 0x31, 0x54, 0x47,
    0xA1, 0xFA, 0x9D, 0x25, 0x7B, 0x02, 0x88, 0x0A, 0xE8, 0x5A, 0xBA, 0x8B,
    0xA2, 0xD3, 0x8A, 0x62, 0x46, 0x8C, 0xEC, 0x52, 0xBE, 0xDE, 0xFC, 0x75,
    0xF5, 0xDB, 0x9C, 0x8C, 0x9B, 0x34, 0x7A, 0xE7, 0x4A, 0x5F, 0xBB, 0x96,
    0x38, 0x19, 0xAB, 0x24
};

static byte a[] = {
    0x37, 0x95, 0xF2, 0xA6, 0xF1, 0x6F, 0x0D, 0x58, 0xBF, 0xED, 0x44, 0x87,
    0xE0, 0xB6, 0xCC, 0x1C, 0xA0, 0x50, 0xC6, 0x61, 0xBB, 0x36, 0xE0, 0x9A,
    0xF3, 0xF7, 0x1E, 0x7A, 0x61, 0x86, 0x5A, 0xF5
};

static byte A[] = {
    0x8D, 0x28, 0xC5, 0x6A, 0x46, 0x5C, 0x82, 0xDB, 0xC7, 0xF6, 0x8B, 0x62,
    0x1A, 0xAD, 0xA1, 0x76, 0x1B, 0x55, 0xFF, 0xAB, 0x10, 0x2F, 0xFF, 0x4A,
    0xAA, 0x46, 0xAD, 0x33, 0x64, 0xDE, 0x28, 0x2E, 0x82, 0x7A, 0xBE, 0xEA,
    0x32, 0xFC, 0xD6, 0x14, 0x01, 0x71, 0xE6, 0xC8, 0xC9, 0x53, 0x69, 0x55,
    0xE1, 0xF8, 0x3D, 0xDD, 0xC7, 0xD5, 0x21, 0xCE, 0xFF, 0x17, 0xFC, 0x23,
    0xBF, 0xCF, 0x2D, 0xB0
};

static byte b[] = {
    0x2B, 0xDD, 0x30, 0x30, 0x53, 0xAF, 0xD8, 0x3A, 0xE7, 0xE0, 0x17, 0x82,
    0x39, 0x44, 0x2C, 0xDB, 0x30, 0x88, 0x0F, 0xC8, 0x88, 0xC2, 0xB2, 0xC1,
    0x78, 0x43, 0x2F, 0xD5, 0x60, 0xD4, 0xDA, 0x43
};

static byte B[] = {
    0xB5, 0x80, 0x36, 0x7F, 0x50, 0x89, 0xC1, 0x04, 0x42, 0x98, 0xD7, 0x6A,
    0x37, 0x8E, 0xF1, 0x81, 0x52, 0xC5, 0x7A, 0xA1, 0xD5, 0xB7, 0x66, 0x84,
    0xA1, 0x3E, 0x32, 0x82, 0x2B, 0x3A, 0xB5, 0xD7, 0x3D, 0x50, 0xF1, 0x58,
    0xBD, 0x89, 0x75, 0xC7, 0x51, 0xCF, 0x6C, 0x03, 0xD4, 0xCA, 0xD5, 0x6E,
    0x97, 0x4D, 0xA3, 0x1E, 0x19, 0x0B, 0xF0, 0xAA, 0x7D, 0x14, 0x90, 0x80,
    0x0E, 0xC7, 0x92, 0xAD
};

static byte key[] = {
    0x66, 0x00, 0x9D, 0x58, 0xB3, 0xD2, 0x0D, 0x4B, 0x69, 0x7F, 0xCF, 0x48,
    0xFF, 0x8F, 0x15, 0x81, 0x4C, 0x4B, 0xFE, 0x9D, 0x85, 0x77, 0x88, 0x60,
    0x1D, 0x1E, 0x51, 0xCF, 0x75, 0xCC, 0x58, 0x00, 0xE7, 0x8D, 0x22, 0x87,
    0x13, 0x6C, 0x88, 0x55
};

static byte client_proof[] = {
    0x0D, 0x49, 0xE1, 0x9C, 0x3A, 0x88, 0x43, 0x15, 0x45, 0xA8, 0xAC, 0xAB,
    0xEA, 0x15, 0x1A, 0xEE, 0xF9, 0x38, 0x4D, 0x21
};

static byte server_proof[] = {
    0xBD, 0xB1, 0x20, 0x70, 0x46, 0xC9, 0xD6, 0xCC, 0xE2, 0x1D, 0x75, 0xA2,
    0xD0, 0xAF, 0xC5, 0xBC, 0xAE, 0x12, 0xFC, 0x75
};

static void test_SrpInit(void)
{
    Srp srp;

    /* invalid params */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpInit(NULL, SRP_TYPE_SHA, SRP_CLIENT_SIDE));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpInit(&srp, (SrpType)255, SRP_CLIENT_SIDE));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpInit(&srp, SRP_TYPE_SHA, (SrpSide)255));

    /* success */
    AssertIntEQ(0, wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE));

    wc_SrpTerm(&srp);
}

static void test_SrpSetUsername(void)
{
    Srp srp;

    AssertIntEQ(0, wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE));

    /* invalid params */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetUsername(NULL, username, usernameSz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetUsername(&srp, NULL, usernameSz));

    /* success */
    AssertIntEQ(0, wc_SrpSetUsername(&srp, username, usernameSz));
    AssertIntEQ((int) usernameSz, srp.userSz);
    AssertIntEQ(0, XMEMCMP(srp.user, username, usernameSz));

    wc_SrpTerm(&srp);
}

static void test_SrpSetParams(void)
{
    Srp srp;

    AssertIntEQ(0, wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE));

    /* invalid call order */
    AssertIntEQ(SRP_CALL_ORDER_E, wc_SrpSetParams(&srp, N,    sizeof(N),
                                                        g,    sizeof(g),
                                                        salt, sizeof(salt)));

    /* fix call order */
    AssertIntEQ(0, wc_SrpSetUsername(&srp, username, usernameSz));

    /* invalid params */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetParams(NULL, N,    sizeof(N),
                                                    g,    sizeof(g),
                                                    salt, sizeof(salt)));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetParams(&srp, NULL, sizeof(N),
                                                    g,    sizeof(g),
                                                    salt, sizeof(salt)));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetParams(&srp, N,    sizeof(N),
                                                    NULL, sizeof(g),
                                                    salt, sizeof(salt)));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetParams(&srp, N,    sizeof(N),
                                                    g,    sizeof(g),
                                                    NULL, sizeof(salt)));

    /* success */
    AssertIntEQ(0, wc_SrpSetParams(&srp, N,    sizeof(N),
                                         g,    sizeof(g),
                                         salt, sizeof(salt)));

    AssertIntEQ(sizeof(salt), srp.saltSz);
    AssertIntEQ(0, XMEMCMP(srp.salt, salt, srp.saltSz));

    wc_SrpTerm(&srp);
}

static void test_SrpSetPassword(void)
{
    Srp srp;
    byte v[64];
    word32 vSz = 0;

    AssertIntEQ(0, wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE));
    AssertIntEQ(0, wc_SrpSetUsername(&srp, username, usernameSz));

    /* invalid call order */
    AssertIntEQ(SRP_CALL_ORDER_E,
                wc_SrpSetPassword(&srp, password, passwordSz));
    AssertIntEQ(SRP_CALL_ORDER_E,
                wc_SrpGetVerifier(&srp, v, &vSz));

    /* fix call order */
    AssertIntEQ(0, wc_SrpSetParams(&srp, N,    sizeof(N),
                                         g,    sizeof(g),
                                         salt, sizeof(salt)));

    /* invalid params */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetPassword(NULL, password, passwordSz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetPassword(&srp, NULL,     passwordSz));

    /* success */
    AssertIntEQ(0, wc_SrpSetPassword(&srp, password, passwordSz));

    /* invalid params */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpGetVerifier(NULL, v,    &vSz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpGetVerifier(&srp, NULL, &vSz));
    AssertIntEQ(BUFFER_E,     wc_SrpGetVerifier(&srp, v,    &vSz));

    /* success */
    vSz = sizeof(v);
    AssertIntEQ(0, wc_SrpGetVerifier(&srp, v, &vSz));
    AssertIntEQ(vSz, sizeof(verifier));
    AssertIntEQ(0, XMEMCMP(verifier, v, vSz));

    /* invalid params - client side srp */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetVerifier(&srp, v, vSz));

    wc_SrpTerm(&srp);
    AssertIntEQ(0, wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_SERVER_SIDE));

    /* invalid params */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetVerifier(NULL, v,    vSz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpSetVerifier(&srp, NULL, vSz));

    /* success */
    AssertIntEQ(0, wc_SrpSetVerifier(&srp, v, vSz));

    wc_SrpTerm(&srp);
}

static void test_SrpGetPublic(void)
{
    Srp srp;
    byte pub[64];
    word32 pubSz = 0;

    AssertIntEQ(0, wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_CLIENT_SIDE));
    AssertIntEQ(0, wc_SrpSetUsername(&srp, username, usernameSz));
    AssertIntEQ(0, wc_SrpSetParams(&srp, N,    sizeof(N),
                                         g,    sizeof(g),
                                         salt, sizeof(salt)));

    /* invalid call order */
    AssertIntEQ(SRP_CALL_ORDER_E, wc_SrpGetPublic(&srp, pub, &pubSz));

    /* fix call order */
    AssertIntEQ(0, wc_SrpSetPassword(&srp, password, passwordSz));

    /* invalid params */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpGetPublic(NULL, pub, &pubSz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpGetPublic(&srp, NULL,   &pubSz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpGetPublic(&srp, pub, NULL));
    AssertIntEQ(BUFFER_E,     wc_SrpGetPublic(&srp, pub, &pubSz));

    /* success */
    pubSz = sizeof(pub);
    AssertIntEQ(0, wc_SrpSetPrivate(&srp, a, sizeof(a)));
    AssertIntEQ(0, wc_SrpGetPublic(&srp, pub, &pubSz));
    AssertIntEQ(pubSz, sizeof(A));
    AssertIntEQ(0, XMEMCMP(pub, A, pubSz));

    wc_SrpTerm(&srp);

    AssertIntEQ(0, wc_SrpInit(&srp, SRP_TYPE_SHA, SRP_SERVER_SIDE));
    AssertIntEQ(0, wc_SrpSetUsername(&srp, username, usernameSz));
    AssertIntEQ(0, wc_SrpSetParams(&srp, N,    sizeof(N),
                                         g,    sizeof(g),
                                         salt, sizeof(salt)));

    /* invalid call order */
    AssertIntEQ(SRP_CALL_ORDER_E, wc_SrpGetPublic(&srp, pub, &pubSz));

    /* fix call order */
    AssertIntEQ(0, wc_SrpSetVerifier(&srp, verifier, sizeof(verifier)));

    /* success */
    AssertIntEQ(0, wc_SrpSetPrivate(&srp, b, sizeof(b)));
    AssertIntEQ(0, wc_SrpGetPublic(&srp, pub, &pubSz));
    AssertIntEQ(pubSz, sizeof(B));
    AssertIntEQ(0, XMEMCMP(pub, B, pubSz));

    wc_SrpTerm(&srp);
}

static void test_SrpComputeKey(void)
{
    Srp cli, srv;
    byte clientPubKey[64];
    byte serverPubKey[64];
    word32 clientPubKeySz = 64;
    word32 serverPubKeySz = 64;

    AssertIntEQ(0, wc_SrpInit(&cli, SRP_TYPE_SHA, SRP_CLIENT_SIDE));
    AssertIntEQ(0, wc_SrpInit(&srv, SRP_TYPE_SHA, SRP_SERVER_SIDE));

    /* invalid call order */
    AssertIntEQ(SRP_CALL_ORDER_E, wc_SrpComputeKey(&cli,
                                                   clientPubKey, clientPubKeySz,
                                                   serverPubKey, serverPubKeySz));

    /* fix call order */
    AssertIntEQ(0, wc_SrpSetUsername(&cli, username, usernameSz));
    AssertIntEQ(0, wc_SrpSetUsername(&srv, username, usernameSz));

    AssertIntEQ(0, wc_SrpSetParams(&cli, N,    sizeof(N),
                                         g,    sizeof(g),
                                         salt, sizeof(salt)));
    AssertIntEQ(0, wc_SrpSetParams(&srv, N,    sizeof(N),
                                         g,    sizeof(g),
                                         salt, sizeof(salt)));

    AssertIntEQ(0, wc_SrpSetPassword(&cli, password, passwordSz));
    AssertIntEQ(0, wc_SrpSetVerifier(&srv, verifier, sizeof(verifier)));

    AssertIntEQ(0, wc_SrpSetPrivate(&cli, a, sizeof(a)));
    AssertIntEQ(0, wc_SrpGetPublic(&cli, clientPubKey, &clientPubKeySz));
    AssertIntEQ(0, XMEMCMP(clientPubKey, A, clientPubKeySz));
    AssertIntEQ(0, wc_SrpSetPrivate(&srv, b, sizeof(b)));
    AssertIntEQ(0, wc_SrpGetPublic(&srv, serverPubKey, &serverPubKeySz));
    AssertIntEQ(0, XMEMCMP(serverPubKey, B, serverPubKeySz));

    /* invalid params */
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpComputeKey(NULL,
                                               clientPubKey, clientPubKeySz,
                                               serverPubKey, serverPubKeySz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpComputeKey(&cli,
                                               NULL,         clientPubKeySz,
                                               serverPubKey, serverPubKeySz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpComputeKey(&cli,
                                               clientPubKey, 0,
                                               serverPubKey, serverPubKeySz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpComputeKey(&cli,
                                               clientPubKey, clientPubKeySz,
                                               NULL,         serverPubKeySz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpComputeKey(&cli,
                                               clientPubKey, clientPubKeySz,
                                               serverPubKey, 0));

    /* success */
    AssertIntEQ(0, wc_SrpComputeKey(&cli, clientPubKey, clientPubKeySz,
                                          serverPubKey, serverPubKeySz));
    AssertIntEQ(0, wc_SrpComputeKey(&srv, clientPubKey, clientPubKeySz,
                                          serverPubKey, serverPubKeySz));
    AssertIntEQ(0, XMEMCMP(cli.key, key, sizeof(key)));
    AssertIntEQ(0, XMEMCMP(srv.key, key, sizeof(key)));

    wc_SrpTerm(&cli);
    wc_SrpTerm(&srv);
}

static void test_SrpGetProofAndVerify(void)
{
    Srp cli, srv;
    byte clientPubKey[64];
    byte serverPubKey[64];
    word32 clientPubKeySz = 64;
    word32 serverPubKeySz = 64;
    byte clientProof[SRP_MAX_DIGEST_SIZE];
    byte serverProof[SRP_MAX_DIGEST_SIZE];
    word32 clientProofSz = SRP_MAX_DIGEST_SIZE;
    word32 serverProofSz = SRP_MAX_DIGEST_SIZE;

    AssertIntEQ(0, wc_SrpInit(&cli, SRP_TYPE_SHA, SRP_CLIENT_SIDE));
    AssertIntEQ(0, wc_SrpInit(&srv, SRP_TYPE_SHA, SRP_SERVER_SIDE));

    AssertIntEQ(0, wc_SrpSetUsername(&cli, username, usernameSz));
    AssertIntEQ(0, wc_SrpSetUsername(&srv, username, usernameSz));

    AssertIntEQ(0, wc_SrpSetParams(&cli, N,    sizeof(N),
                                         g,    sizeof(g),
                                         salt, sizeof(salt)));
    AssertIntEQ(0, wc_SrpSetParams(&srv, N,    sizeof(N),
                                         g,    sizeof(g),
                                         salt, sizeof(salt)));

    AssertIntEQ(0, wc_SrpSetPassword(&cli, password, passwordSz));
    AssertIntEQ(0, wc_SrpSetVerifier(&srv, verifier, sizeof(verifier)));

    AssertIntEQ(0, wc_SrpSetPrivate(&cli, a, sizeof(a)));
    AssertIntEQ(0, wc_SrpGetPublic(&cli, clientPubKey, &clientPubKeySz));
    AssertIntEQ(0, XMEMCMP(clientPubKey, A, clientPubKeySz));

    AssertIntEQ(0, wc_SrpSetPrivate(&srv, b, sizeof(b)));
    AssertIntEQ(0, wc_SrpGetPublic(&srv, serverPubKey, &serverPubKeySz));
    AssertIntEQ(0, XMEMCMP(serverPubKey, B, serverPubKeySz));

    AssertIntEQ(0, wc_SrpComputeKey(&cli, clientPubKey, clientPubKeySz,
                                          serverPubKey, serverPubKeySz));
    AssertIntEQ(0, XMEMCMP(cli.key, key, sizeof(key)));

    AssertIntEQ(0, wc_SrpComputeKey(&srv, clientPubKey, clientPubKeySz,
                                          serverPubKey, serverPubKeySz));
    AssertIntEQ(0, XMEMCMP(srv.key, key, sizeof(key)));

    /* invalid params */
    serverProofSz = 0;
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpGetProof(NULL, clientProof,&clientProofSz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpGetProof(&cli, NULL,       &clientProofSz));
    AssertIntEQ(BAD_FUNC_ARG, wc_SrpGetProof(&cli, clientProof,NULL));
    AssertIntEQ(BUFFER_E,     wc_SrpGetProof(&srv, serverProof,&serverProofSz));

    AssertIntEQ(BAD_FUNC_ARG,
                wc_SrpVerifyPeersProof(NULL, clientProof, clientProofSz));
    AssertIntEQ(BAD_FUNC_ARG,
                wc_SrpVerifyPeersProof(&cli, NULL,        clientProofSz));
    AssertIntEQ(BUFFER_E,
                wc_SrpVerifyPeersProof(&srv, serverProof, serverProofSz));
    serverProofSz = SRP_MAX_DIGEST_SIZE;

    /* success */
    AssertIntEQ(0, wc_SrpGetProof(&cli, clientProof, &clientProofSz));
    AssertIntEQ(0, XMEMCMP(clientProof, client_proof, sizeof(client_proof)));
    AssertIntEQ(0, wc_SrpVerifyPeersProof(&srv, clientProof, clientProofSz));
    AssertIntEQ(0, wc_SrpGetProof(&srv, serverProof, &serverProofSz));
    AssertIntEQ(0, XMEMCMP(serverProof, server_proof, sizeof(server_proof)));
    AssertIntEQ(0, wc_SrpVerifyPeersProof(&cli, serverProof, serverProofSz));

    wc_SrpTerm(&cli);
    wc_SrpTerm(&srv);
}

static int sha512_key_gen(Srp* srp, byte* secret, word32 size)
{
    wc_Sha512 hash;
    int r;

    srp->key = (byte*)XMALLOC(WC_SHA512_DIGEST_SIZE, NULL, DYNAMIC_TYPE_SRP);
    if (srp->key == NULL)
        return MEMORY_E;

    srp->keySz = WC_SHA512_DIGEST_SIZE;

    r = wc_InitSha512(&hash);
    if (!r) r = wc_Sha512Update(&hash, secret, size);
    if (!r) r = wc_Sha512Final(&hash, srp->key);

    XMEMSET(&hash, 0, sizeof(wc_Sha512));

    return r;
}

static void test_SrpKeyGenFunc_cb(void)
{
    Srp cli, srv;
    byte clientPubKey[1024];
    byte serverPubKey[1024];
    word32 clientPubKeySz = 1024;
    word32 serverPubKeySz = 1024;
    byte clientProof[SRP_MAX_DIGEST_SIZE];
    byte serverProof[SRP_MAX_DIGEST_SIZE];
    word32 clientProofSz = SRP_MAX_DIGEST_SIZE;
    word32 serverProofSz = SRP_MAX_DIGEST_SIZE;

    byte username_[] = "alice";
    word32 usernameSz_ = 5;

    byte password_[] = "password123";
    word32 passwordSz_ = 11;

#if !defined(FP_MAX_BITS) || FP_MAX_BITS > 3072 * 2
    byte N_[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2,
        0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
        0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6,
        0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
        0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
        0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
        0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9,
        0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
        0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
        0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
        0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05, 0x98, 0xDA, 0x48, 0x36,
        0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
        0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56,
        0x20, 0x85, 0x52, 0xBB, 0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
        0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04, 0xF1, 0x74, 0x6C, 0x08,
        0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
        0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2,
        0xEC, 0x07, 0xA2, 0x8F, 0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9,
        0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18, 0x39, 0x95, 0x49, 0x7C,
        0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
        0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAA, 0xC4, 0x2D, 0xAD, 0x33, 0x17, 0x0D,
        0x04, 0x50, 0x7A, 0x33, 0xA8, 0x55, 0x21, 0xAB, 0xDF, 0x1C, 0xBA, 0x64,
        0xEC, 0xFB, 0x85, 0x04, 0x58, 0xDB, 0xEF, 0x0A, 0x8A, 0xEA, 0x71, 0x57,
        0x5D, 0x06, 0x0C, 0x7D, 0xB3, 0x97, 0x0F, 0x85, 0xA6, 0xE1, 0xE4, 0xC7,
        0xAB, 0xF5, 0xAE, 0x8C, 0xDB, 0x09, 0x33, 0xD7, 0x1E, 0x8C, 0x94, 0xE0,
        0x4A, 0x25, 0x61, 0x9D, 0xCE, 0xE3, 0xD2, 0x26, 0x1A, 0xD2, 0xEE, 0x6B,
        0xF1, 0x2F, 0xFA, 0x06, 0xD9, 0x8A, 0x08, 0x64, 0xD8, 0x76, 0x02, 0x73,
        0x3E, 0xC8, 0x6A, 0x64, 0x52, 0x1F, 0x2B, 0x18, 0x17, 0x7B, 0x20, 0x0C,
        0xBB, 0xE1, 0x17, 0x57, 0x7A, 0x61, 0x5D, 0x6C, 0x77, 0x09, 0x88, 0xC0,
        0xBA, 0xD9, 0x46, 0xE2, 0x08, 0xE2, 0x4F, 0xA0, 0x74, 0xE5, 0xAB, 0x31,
        0x43, 0xDB, 0x5B, 0xFC, 0xE0, 0xFD, 0x10, 0x8E, 0x4B, 0x82, 0xD1, 0x20,
        0xA9, 0x3A, 0xD2, 0xCA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    byte g_[] = {
        0x05
    };

    byte salt_[] = {
        0xBE, 0xB2, 0x53, 0x79, 0xD1, 0xA8, 0x58, 0x1E, 0xB5, 0xA7, 0x27, 0x67,
        0x3A, 0x24, 0x41, 0xEE
    };

    byte verifier_[] = {
        0x9B, 0x5E, 0x06, 0x17, 0x01, 0xEA, 0x7A, 0xEB, 0x39, 0xCF, 0x6E, 0x35,
        0x19, 0x65, 0x5A, 0x85, 0x3C, 0xF9, 0x4C, 0x75, 0xCA, 0xF2, 0x55, 0x5E,
        0xF1, 0xFA, 0xF7, 0x59, 0xBB, 0x79, 0xCB, 0x47, 0x70, 0x14, 0xE0, 0x4A,
        0x88, 0xD6, 0x8F, 0xFC, 0x05, 0x32, 0x38, 0x91, 0xD4, 0xC2, 0x05, 0xB8,
        0xDE, 0x81, 0xC2, 0xF2, 0x03, 0xD8, 0xFA, 0xD1, 0xB2, 0x4D, 0x2C, 0x10,
        0x97, 0x37, 0xF1, 0xBE, 0xBB, 0xD7, 0x1F, 0x91, 0x24, 0x47, 0xC4, 0xA0,
        0x3C, 0x26, 0xB9, 0xFA, 0xD8, 0xED, 0xB3, 0xE7, 0x80, 0x77, 0x8E, 0x30,
        0x25, 0x29, 0xED, 0x1E, 0xE1, 0x38, 0xCC, 0xFC, 0x36, 0xD4, 0xBA, 0x31,
        0x3C, 0xC4, 0x8B, 0x14, 0xEA, 0x8C, 0x22, 0xA0, 0x18, 0x6B, 0x22, 0x2E,
        0x65, 0x5F, 0x2D, 0xF5, 0x60, 0x3F, 0xD7, 0x5D, 0xF7, 0x6B, 0x3B, 0x08,
        0xFF, 0x89, 0x50, 0x06, 0x9A, 0xDD, 0x03, 0xA7, 0x54, 0xEE, 0x4A, 0xE8,
        0x85, 0x87, 0xCC, 0xE1, 0xBF, 0xDE, 0x36, 0x79, 0x4D, 0xBA, 0xE4, 0x59,
        0x2B, 0x7B, 0x90, 0x4F, 0x44, 0x2B, 0x04, 0x1C, 0xB1, 0x7A, 0xEB, 0xAD,
        0x1E, 0x3A, 0xEB, 0xE3, 0xCB, 0xE9, 0x9D, 0xE6, 0x5F, 0x4B, 0xB1, 0xFA,
        0x00, 0xB0, 0xE7, 0xAF, 0x06, 0x86, 0x3D, 0xB5, 0x3B, 0x02, 0x25, 0x4E,
        0xC6, 0x6E, 0x78, 0x1E, 0x3B, 0x62, 0xA8, 0x21, 0x2C, 0x86, 0xBE, 0xB0,
        0xD5, 0x0B, 0x5B, 0xA6, 0xD0, 0xB4, 0x78, 0xD8, 0xC4, 0xE9, 0xBB, 0xCE,
        0xC2, 0x17, 0x65, 0x32, 0x6F, 0xBD, 0x14, 0x05, 0x8D, 0x2B, 0xBD, 0xE2,
        0xC3, 0x30, 0x45, 0xF0, 0x38, 0x73, 0xE5, 0x39, 0x48, 0xD7, 0x8B, 0x79,
        0x4F, 0x07, 0x90, 0xE4, 0x8C, 0x36, 0xAE, 0xD6, 0xE8, 0x80, 0xF5, 0x57,
        0x42, 0x7B, 0x2F, 0xC0, 0x6D, 0xB5, 0xE1, 0xE2, 0xE1, 0xD7, 0xE6, 0x61,
        0xAC, 0x48, 0x2D, 0x18, 0xE5, 0x28, 0xD7, 0x29, 0x5E, 0xF7, 0x43, 0x72,
        0x95, 0xFF, 0x1A, 0x72, 0xD4, 0x02, 0x77, 0x17, 0x13, 0xF1, 0x68, 0x76,
        0xDD, 0x05, 0x0A, 0xE5, 0xB7, 0xAD, 0x53, 0xCC, 0xB9, 0x08, 0x55, 0xC9,
        0x39, 0x56, 0x64, 0x83, 0x58, 0xAD, 0xFD, 0x96, 0x64, 0x22, 0xF5, 0x24,
        0x98, 0x73, 0x2D, 0x68, 0xD1, 0xD7, 0xFB, 0xEF, 0x10, 0xD7, 0x80, 0x34,
        0xAB, 0x8D, 0xCB, 0x6F, 0x0F, 0xCF, 0x88, 0x5C, 0xC2, 0xB2, 0xEA, 0x2C,
        0x3E, 0x6A, 0xC8, 0x66, 0x09, 0xEA, 0x05, 0x8A, 0x9D, 0xA8, 0xCC, 0x63,
        0x53, 0x1D, 0xC9, 0x15, 0x41, 0x4D, 0xF5, 0x68, 0xB0, 0x94, 0x82, 0xDD,
        0xAC, 0x19, 0x54, 0xDE, 0xC7, 0xEB, 0x71, 0x4F, 0x6F, 0xF7, 0xD4, 0x4C,
        0xD5, 0xB8, 0x6F, 0x6B, 0xD1, 0x15, 0x81, 0x09, 0x30, 0x63, 0x7C, 0x01,
        0xD0, 0xF6, 0x01, 0x3B, 0xC9, 0x74, 0x0F, 0xA2, 0xC6, 0x33, 0xBA, 0x89
    };

    byte a_[] = {
        0x60, 0x97, 0x55, 0x27, 0x03, 0x5C, 0xF2, 0xAD, 0x19, 0x89, 0x80, 0x6F,
        0x04, 0x07, 0x21, 0x0B, 0xC8, 0x1E, 0xDC, 0x04, 0xE2, 0x76, 0x2A, 0x56,
        0xAF, 0xD5, 0x29, 0xDD, 0xDA, 0x2D, 0x43, 0x93
    };

    byte A_[] = {
        0xFA, 0xB6, 0xF5, 0xD2, 0x61, 0x5D, 0x1E, 0x32, 0x35, 0x12, 0xE7, 0x99,
        0x1C, 0xC3, 0x74, 0x43, 0xF4, 0x87, 0xDA, 0x60, 0x4C, 0xA8, 0xC9, 0x23,
        0x0F, 0xCB, 0x04, 0xE5, 0x41, 0xDC, 0xE6, 0x28, 0x0B, 0x27, 0xCA, 0x46,
        0x80, 0xB0, 0x37, 0x4F, 0x17, 0x9D, 0xC3, 0xBD, 0xC7, 0x55, 0x3F, 0xE6,
        0x24, 0x59, 0x79, 0x8C, 0x70, 0x1A, 0xD8, 0x64, 0xA9, 0x13, 0x90, 0xA2,
        0x8C, 0x93, 0xB6, 0x44, 0xAD, 0xBF, 0x9C, 0x00, 0x74, 0x5B, 0x94, 0x2B,
        0x79, 0xF9, 0x01, 0x2A, 0x21, 0xB9, 0xB7, 0x87, 0x82, 0x31, 0x9D, 0x83,
        0xA1, 0xF8, 0x36, 0x28, 0x66, 0xFB, 0xD6, 0xF4, 0x6B, 0xFC, 0x0D, 0xDB,
        0x2E, 0x1A, 0xB6, 0xE4, 0xB4, 0x5A, 0x99, 0x06, 0xB8, 0x2E, 0x37, 0xF0,
        0x5D, 0x6F, 0x97, 0xF6, 0xA3, 0xEB, 0x6E, 0x18, 0x20, 0x79, 0x75, 0x9C,
        0x4F, 0x68, 0x47, 0x83, 0x7B, 0x62, 0x32, 0x1A, 0xC1, 0xB4, 0xFA, 0x68,
        0x64, 0x1F, 0xCB, 0x4B, 0xB9, 0x8D, 0xD6, 0x97, 0xA0, 0xC7, 0x36, 0x41,
        0x38, 0x5F, 0x4B, 0xAB, 0x25, 0xB7, 0x93, 0x58, 0x4C, 0xC3, 0x9F, 0xC8,
        0xD4, 0x8D, 0x4B, 0xD8, 0x67, 0xA9, 0xA3, 0xC1, 0x0F, 0x8E, 0xA1, 0x21,
        0x70, 0x26, 0x8E, 0x34, 0xFE, 0x3B, 0xBE, 0x6F, 0xF8, 0x99, 0x98, 0xD6,
        0x0D, 0xA2, 0xF3, 0xE4, 0x28, 0x3C, 0xBE, 0xC1, 0x39, 0x3D, 0x52, 0xAF,
        0x72, 0x4A, 0x57, 0x23, 0x0C, 0x60, 0x4E, 0x9F, 0xBC, 0xE5, 0x83, 0xD7,
        0x61, 0x3E, 0x6B, 0xFF, 0xD6, 0x75, 0x96, 0xAD, 0x12, 0x1A, 0x87, 0x07,
        0xEE, 0xC4, 0x69, 0x44, 0x95, 0x70, 0x33, 0x68, 0x6A, 0x15, 0x5F, 0x64,
        0x4D, 0x5C, 0x58, 0x63, 0xB4, 0x8F, 0x61, 0xBD, 0xBF, 0x19, 0xA5, 0x3E,
        0xAB, 0x6D, 0xAD, 0x0A, 0x18, 0x6B, 0x8C, 0x15, 0x2E, 0x5F, 0x5D, 0x8C,
        0xAD, 0x4B, 0x0E, 0xF8, 0xAA, 0x4E, 0xA5, 0x00, 0x88, 0x34, 0xC3, 0xCD,
        0x34, 0x2E, 0x5E, 0x0F, 0x16, 0x7A, 0xD0, 0x45, 0x92, 0xCD, 0x8B, 0xD2,
        0x79, 0x63, 0x93, 0x98, 0xEF, 0x9E, 0x11, 0x4D, 0xFA, 0xAA, 0xB9, 0x19,
        0xE1, 0x4E, 0x85, 0x09, 0x89, 0x22, 0x4D, 0xDD, 0x98, 0x57, 0x6D, 0x79,
        0x38, 0x5D, 0x22, 0x10, 0x90, 0x2E, 0x9F, 0x9B, 0x1F, 0x2D, 0x86, 0xCF,
        0xA4, 0x7E, 0xE2, 0x44, 0x63, 0x54, 0x65, 0xF7, 0x10, 0x58, 0x42, 0x1A,
        0x01, 0x84, 0xBE, 0x51, 0xDD, 0x10, 0xCC, 0x9D, 0x07, 0x9E, 0x6F, 0x16,
        0x04, 0xE7, 0xAA, 0x9B, 0x7C, 0xF7, 0x88, 0x3C, 0x7D, 0x4C, 0xE1, 0x2B,
        0x06, 0xEB, 0xE1, 0x60, 0x81, 0xE2, 0x3F, 0x27, 0xA2, 0x31, 0xD1, 0x84,
        0x32, 0xD7, 0xD1, 0xBB, 0x55, 0xC2, 0x8A, 0xE2, 0x1F, 0xFC, 0xF0, 0x05,
        0xF5, 0x75, 0x28, 0xD1, 0x5A, 0x88, 0x88, 0x1B, 0xB3, 0xBB, 0xB7, 0xFE
    };

    byte b_[] = {
        0xE4, 0x87, 0xCB, 0x59, 0xD3, 0x1A, 0xC5, 0x50, 0x47, 0x1E, 0x81, 0xF0,
        0x0F, 0x69, 0x28, 0xE0, 0x1D, 0xDA, 0x08, 0xE9, 0x74, 0xA0, 0x04, 0xF4,
        0x9E, 0x61, 0xF5, 0xD1, 0x05, 0x28, 0x4D, 0x20
    };

    byte B_[] = {
        0x40, 0xF5, 0x70, 0x88, 0xA4, 0x82, 0xD4, 0xC7, 0x73, 0x33, 0x84, 0xFE,
        0x0D, 0x30, 0x1F, 0xDD, 0xCA, 0x90, 0x80, 0xAD, 0x7D, 0x4F, 0x6F, 0xDF,
        0x09, 0xA0, 0x10, 0x06, 0xC3, 0xCB, 0x6D, 0x56, 0x2E, 0x41, 0x63, 0x9A,
        0xE8, 0xFA, 0x21, 0xDE, 0x3B, 0x5D, 0xBA, 0x75, 0x85, 0xB2, 0x75, 0x58,
        0x9B, 0xDB, 0x27, 0x98, 0x63, 0xC5, 0x62, 0x80, 0x7B, 0x2B, 0x99, 0x08,
        0x3C, 0xD1, 0x42, 0x9C, 0xDB, 0xE8, 0x9E, 0x25, 0xBF, 0xBD, 0x7E, 0x3C,
        0xAD, 0x31, 0x73, 0xB2, 0xE3, 0xC5, 0xA0, 0xB1, 0x74, 0xDA, 0x6D, 0x53,
        0x91, 0xE6, 0xA0, 0x6E, 0x46, 0x5F, 0x03, 0x7A, 0x40, 0x06, 0x25, 0x48,
        0x39, 0xA5, 0x6B, 0xF7, 0x6D, 0xA8, 0x4B, 0x1C, 0x94, 0xE0, 0xAE, 0x20,
        0x85, 0x76, 0x15, 0x6F, 0xE5, 0xC1, 0x40, 0xA4, 0xBA, 0x4F, 0xFC, 0x9E,
        0x38, 0xC3, 0xB0, 0x7B, 0x88, 0x84, 0x5F, 0xC6, 0xF7, 0xDD, 0xDA, 0x93,
        0x38, 0x1F, 0xE0, 0xCA, 0x60, 0x84, 0xC4, 0xCD, 0x2D, 0x33, 0x6E, 0x54,
        0x51, 0xC4, 0x64, 0xCC, 0xB6, 0xEC, 0x65, 0xE7, 0xD1, 0x6E, 0x54, 0x8A,
        0x27, 0x3E, 0x82, 0x62, 0x84, 0xAF, 0x25, 0x59, 0xB6, 0x26, 0x42, 0x74,
        0x21, 0x59, 0x60, 0xFF, 0xF4, 0x7B, 0xDD, 0x63, 0xD3, 0xAF, 0xF0, 0x64,
        0xD6, 0x13, 0x7A, 0xF7, 0x69, 0x66, 0x1C, 0x9D, 0x4F, 0xEE, 0x47, 0x38,
        0x26, 0x03, 0xC8, 0x8E, 0xAA, 0x09, 0x80, 0x58, 0x1D, 0x07, 0x75, 0x84,
        0x61, 0xB7, 0x77, 0xE4, 0x35, 0x6D, 0xDA, 0x58, 0x35, 0x19, 0x8B, 0x51,
        0xFE, 0xEA, 0x30, 0x8D, 0x70, 0xF7, 0x54, 0x50, 0xB7, 0x16, 0x75, 0xC0,
        0x8C, 0x7D, 0x83, 0x02, 0xFD, 0x75, 0x39, 0xDD, 0x1F, 0xF2, 0xA1, 0x1C,
        0xB4, 0x25, 0x8A, 0xA7, 0x0D, 0x23, 0x44, 0x36, 0xAA, 0x42, 0xB6, 0xA0,
        0x61, 0x5F, 0x3F, 0x91, 0x5D, 0x55, 0xCC, 0x3B, 0x96, 0x6B, 0x27, 0x16,
        0xB3, 0x6E, 0x4D, 0x1A, 0x06, 0xCE, 0x5E, 0x5D, 0x2E, 0xA3, 0xBE, 0xE5,
        0xA1, 0x27, 0x0E, 0x87, 0x51, 0xDA, 0x45, 0xB6, 0x0B, 0x99, 0x7B, 0x0F,
        0xFD, 0xB0, 0xF9, 0x96, 0x2F, 0xEE, 0x4F, 0x03, 0xBE, 0xE7, 0x80, 0xBA,
        0x0A, 0x84, 0x5B, 0x1D, 0x92, 0x71, 0x42, 0x17, 0x83, 0xAE, 0x66, 0x01,
        0xA6, 0x1E, 0xA2, 0xE3, 0x42, 0xE4, 0xF2, 0xE8, 0xBC, 0x93, 0x5A, 0x40,
        0x9E, 0xAD, 0x19, 0xF2, 0x21, 0xBD, 0x1B, 0x74, 0xE2, 0x96, 0x4D, 0xD1,
        0x9F, 0xC8, 0x45, 0xF6, 0x0E, 0xFC, 0x09, 0x33, 0x8B, 0x60, 0xB6, 0xB2,
        0x56, 0xD8, 0xCA, 0xC8, 0x89, 0xCC, 0xA3, 0x06, 0xCC, 0x37, 0x0A, 0x0B,
        0x18, 0xC8, 0xB8, 0x86, 0xE9, 0x5D, 0xA0, 0xAF, 0x52, 0x35, 0xFE, 0xF4,
        0x39, 0x30, 0x20, 0xD2, 0xB7, 0xF3, 0x05, 0x69, 0x04, 0x75, 0x90, 0x42
    };

    byte key_[] = {
        0x5C, 0xBC, 0x21, 0x9D, 0xB0, 0x52, 0x13, 0x8E, 0xE1, 0x14, 0x8C, 0x71,
        0xCD, 0x44, 0x98, 0x96, 0x3D, 0x68, 0x25, 0x49, 0xCE, 0x91, 0xCA, 0x24,
        0xF0, 0x98, 0x46, 0x8F, 0x06, 0x01, 0x5B, 0xEB, 0x6A, 0xF2, 0x45, 0xC2,
        0x09, 0x3F, 0x98, 0xC3, 0x65, 0x1B, 0xCA, 0x83, 0xAB, 0x8C, 0xAB, 0x2B,
        0x58, 0x0B, 0xBF, 0x02, 0x18, 0x4F, 0xEF, 0xDF, 0x26, 0x14, 0x2F, 0x73,
        0xDF, 0x95, 0xAC, 0x50
    };
#else
    byte N_[] = {
        0xAC, 0x6B, 0xDB, 0x41, 0x32, 0x4A, 0x9A, 0x9B, 0xF1, 0x66, 0xDE, 0x5E,
        0x13, 0x89, 0x58, 0x2F, 0xAF, 0x72, 0xB6, 0x65, 0x19, 0x87, 0xEE, 0x07,
        0xFC, 0x31, 0x92, 0x94, 0x3D, 0xB5, 0x60, 0x50, 0xA3, 0x73, 0x29, 0xCB,
        0xB4, 0xA0, 0x99, 0xED, 0x81, 0x93, 0xE0, 0x75, 0x77, 0x67, 0xA1, 0x3D,
        0xD5, 0x23, 0x12, 0xAB, 0x4B, 0x03, 0x31, 0x0D, 0xCD, 0x7F, 0x48, 0xA9,
        0xDA, 0x04, 0xFD, 0x50, 0xE8, 0x08, 0x39, 0x69, 0xED, 0xB7, 0x67, 0xB0,
        0xCF, 0x60, 0x95, 0x17, 0x9A, 0x16, 0x3A, 0xB3, 0x66, 0x1A, 0x05, 0xFB,
        0xD5, 0xFA, 0xAA, 0xE8, 0x29, 0x18, 0xA9, 0x96, 0x2F, 0x0B, 0x93, 0xB8,
        0x55, 0xF9, 0x79, 0x93, 0xEC, 0x97, 0x5E, 0xEA, 0xA8, 0x0D, 0x74, 0x0A,
        0xDB, 0xF4, 0xFF, 0x74, 0x73, 0x59, 0xD0, 0x41, 0xD5, 0xC3, 0x3E, 0xA7,
        0x1D, 0x28, 0x1E, 0x44, 0x6B, 0x14, 0x77, 0x3B, 0xCA, 0x97, 0xB4, 0x3A,
        0x23, 0xFB, 0x80, 0x16, 0x76, 0xBD, 0x20, 0x7A, 0x43, 0x6C, 0x64, 0x81,
        0xF1, 0xD2, 0xB9, 0x07, 0x87, 0x17, 0x46, 0x1A, 0x5B, 0x9D, 0x32, 0xE6,
        0x88, 0xF8, 0x77, 0x48, 0x54, 0x45, 0x23, 0xB5, 0x24, 0xB0, 0xD5, 0x7D,
        0x5E, 0xA7, 0x7A, 0x27, 0x75, 0xD2, 0xEC, 0xFA, 0x03, 0x2C, 0xFB, 0xDB,
        0xF5, 0x2F, 0xB3, 0x78, 0x61, 0x60, 0x27, 0x90, 0x04, 0xE5, 0x7A, 0xE6,
        0xAF, 0x87, 0x4E, 0x73, 0x03, 0xCE, 0x53, 0x29, 0x9C, 0xCC, 0x04, 0x1C,
        0x7B, 0xC3, 0x08, 0xD8, 0x2A, 0x56, 0x98, 0xF3, 0xA8, 0xD0, 0xC3, 0x82,
        0x71, 0xAE, 0x35, 0xF8, 0xE9, 0xDB, 0xFB, 0xB6, 0x94, 0xB5, 0xC8, 0x03,
        0xD8, 0x9F, 0x7A, 0xE4, 0x35, 0xDE, 0x23, 0x6D, 0x52, 0x5F, 0x54, 0x75,
        0x9B, 0x65, 0xE3, 0x72, 0xFC, 0xD6, 0x8E, 0xF2, 0x0F, 0xA7, 0x11, 0x1F,
        0x9E, 0x4A, 0xFF, 0x73
    };

    byte g_[] = {
        0x02
    };

    byte salt_[] = {
        0xBE, 0xB2, 0x53, 0x79, 0xD1, 0xA8, 0x58, 0x1E, 0xB5, 0xA7, 0x27, 0x67,
        0x3A, 0x24, 0x41, 0xEE
    };

    byte verifier_[] = {
        0xab, 0x58, 0xc3, 0x49, 0x79, 0xda, 0x9c, 0x1c, 0x0c, 0x5f, 0x6a, 0xe8,
        0xa1, 0xc4, 0x20, 0x78, 0x16, 0xe0, 0x29, 0x6f, 0xdc, 0x09, 0x62, 0xe7,
        0x5d, 0x49, 0x09, 0xd2, 0xd3, 0x48, 0x0b, 0x03, 0xbb, 0xf4, 0x96, 0x58,
        0x93, 0xa5, 0x69, 0xd4, 0x89, 0x10, 0x7f, 0xe2, 0x23, 0x73, 0xf2, 0x8c,
        0xc6, 0x5b, 0x52, 0x5d, 0x2a, 0x2b, 0xa5, 0x23, 0x27, 0xa6, 0x5c, 0xf7,
        0x8d, 0x65, 0x1f, 0xcf, 0x5e, 0x18, 0x70, 0xb7, 0xfd, 0x08, 0x4e, 0xa8,
        0x42, 0xbe, 0x5f, 0x73, 0xf4, 0x8b, 0xa0, 0x4a, 0xf6, 0xfa, 0x85, 0xd2,
        0xd7, 0xf6, 0x4f, 0xc0, 0xc5, 0xba, 0x42, 0x32, 0xc5, 0xbb, 0x57, 0xb2,
        0x60, 0xdb, 0xc4, 0x27, 0x28, 0x4a, 0x7c, 0xdc, 0x5f, 0x64, 0x8a, 0xc2,
        0x51, 0x47, 0x96, 0xd9, 0x9f, 0xd7, 0x64, 0x29, 0x30, 0xe3, 0x64, 0xfa,
        0xd2, 0x42, 0xfa, 0x8b, 0xc9, 0xa0, 0x78, 0xc5, 0xcf, 0x3b, 0x1b, 0x60,
        0x2a, 0xcd, 0x35, 0x8b, 0xfd, 0xbd, 0x62, 0xd6, 0x3b, 0x22, 0x29, 0x1d,
        0xb0, 0xeb, 0xaa, 0x00, 0xdf, 0x99, 0x1f, 0x82, 0xef, 0x9d, 0x1a, 0xad,
        0xe2, 0x4d, 0xa1, 0xca, 0xb2, 0x8f, 0x1d, 0xed, 0x5e, 0xc2, 0xbc, 0x48,
        0xae, 0xe8, 0x5f, 0x52, 0xad, 0xb3, 0xcb, 0xa5, 0x4e, 0xff, 0x52, 0x8c,
        0x9b, 0x11, 0x58, 0xe2, 0xd9, 0x94, 0xcd, 0x6c, 0x66, 0x87, 0xf8, 0xc1,
        0x3c, 0x94, 0x87, 0xce, 0x2c, 0x4a, 0x05, 0x66, 0xc3, 0x20, 0x62, 0x67,
        0x79, 0xc0, 0x33, 0xf1, 0x6e, 0x3f, 0xee, 0xc8, 0xbe, 0x8e, 0x56, 0xc6,
        0x98, 0x72, 0x0e, 0x37, 0x4b, 0x89, 0x4c, 0xba, 0xd9, 0x45, 0xf4, 0x9e,
        0x78, 0x7a, 0x6e, 0x1e, 0xdf, 0xff, 0x23, 0xcf, 0xef, 0xdc, 0x7d, 0x1c,
        0xf1, 0x96, 0x70, 0x2a, 0xa8, 0x6c, 0x20, 0xde, 0xce, 0x6e, 0x3e, 0x89,
        0x11, 0x44, 0x2b, 0x02
    };

    byte a_[] = {
        0x60, 0x97, 0x55, 0x27, 0x03, 0x5C, 0xF2, 0xAD, 0x19, 0x89, 0x80, 0x6F,
        0x04, 0x07, 0x21, 0x0B, 0xC8, 0x1E, 0xDC, 0x04, 0xE2, 0x76, 0x2A, 0x56,
        0xAF, 0xD5, 0x29, 0xDD, 0xDA, 0x2D, 0x43, 0x93
    };

    byte A_[] = {
        0x4b, 0x70, 0x0f, 0x8d, 0x48, 0xe6, 0x9c, 0x9a, 0xae, 0x40, 0xc6, 0x84,
        0xac, 0x7c, 0x7c, 0x03, 0x12, 0x1e, 0x2b, 0x76, 0x02, 0xeb, 0x4c, 0x35,
        0x14, 0x80, 0x4c, 0xca, 0xda, 0x0e, 0xd4, 0x01, 0x91, 0x93, 0xa3, 0x51,
        0xec, 0xc6, 0x5a, 0x6f, 0x85, 0x4e, 0xde, 0x91, 0xeb, 0x09, 0x6e, 0x72,
        0x1b, 0x22, 0xd7, 0x01, 0xc7, 0xad, 0xc6, 0x4e, 0x9c, 0xed, 0xac, 0xd7,
        0x5f, 0x2e, 0x26, 0xbb, 0x2f, 0x5e, 0x45, 0xdd, 0x53, 0xdc, 0x8d, 0xbe,
        0xaf, 0xff, 0xe8, 0x2a, 0xa4, 0x9f, 0xca, 0x05, 0x73, 0x44, 0x46, 0x91,
        0x21, 0x25, 0x37, 0xa7, 0x3c, 0xf8, 0x0e, 0x25, 0x03, 0x92, 0x58, 0x20,
        0x5a, 0x7e, 0xdf, 0x47, 0x49, 0xb3, 0x0a, 0xda, 0xf2, 0x58, 0x77, 0xc6,
        0x2f, 0xcd, 0x09, 0xd6, 0x61, 0x35, 0x98, 0xbc, 0xd4, 0xba, 0xf2, 0xa9,
        0x72, 0x7a, 0x53, 0x70, 0x6a, 0x27, 0x81, 0x48, 0x99, 0x2b, 0x2a, 0xbb,
        0x23, 0xad, 0x5d, 0x51, 0x2d, 0x26, 0x9e, 0x16, 0xca, 0x11, 0xbc, 0x08,
        0x95, 0xb5, 0xa3, 0xb5, 0xec, 0x47, 0x21, 0xcd, 0xe4, 0x0a, 0x8c, 0x39,
        0xc7, 0x96, 0xe9, 0x4f, 0x0b, 0xe8, 0x6d, 0xbb, 0xeb, 0x33, 0xda, 0x70,
        0x37, 0x01, 0x89, 0x83, 0x92, 0x1a, 0xba, 0x3f, 0x50, 0x53, 0x19, 0x5d,
        0x5a, 0xc1, 0xda, 0x4e, 0x56, 0x7e, 0x3c, 0x0e, 0x75, 0xd9, 0xe0, 0x60,
        0x9f, 0x92, 0xe8, 0x50, 0x65, 0x7b, 0x2b, 0xe4, 0x77, 0x1f, 0x41, 0x5b,
        0x9c, 0xac, 0xc5, 0xc1, 0xec, 0xed, 0xc3, 0x01, 0x33, 0xbf, 0x64, 0x74,
        0xf5, 0x02, 0x2c, 0x65, 0x19, 0xd7, 0x80, 0x76, 0x0c, 0xa4, 0xd8, 0xd3,
        0xb9, 0x66, 0xb0, 0x34, 0xbd, 0x73, 0x87, 0x7c, 0x1b, 0x3b, 0x33, 0xf4,
        0x74, 0xb9, 0xc3, 0xc5, 0x29, 0x9a, 0x19, 0x68, 0xf3, 0xe6, 0xcd, 0x3b,
        0xfe, 0x84, 0x44, 0x5a
    };

    byte b_[] = {
        0xE4, 0x87, 0xCB, 0x59, 0xD3, 0x1A, 0xC5, 0x50, 0x47, 0x1E, 0x81, 0xF0,
        0x0F, 0x69, 0x28, 0xE0, 0x1D, 0xDA, 0x08, 0xE9, 0x74, 0xA0, 0x04, 0xF4,
        0x9E, 0x61, 0xF5, 0xD1, 0x05, 0x28, 0x4D, 0x20
    };

    byte B_[] = {
        0x03, 0xaf, 0xa6, 0xe2, 0x14, 0xf3, 0x63, 0x35, 0xf5, 0x51, 0x1d, 0xae,
        0x57, 0x34, 0x24, 0xf4, 0xc2, 0x18, 0xaf, 0x11, 0xf8, 0xef, 0xc7, 0x14,
        0x44, 0xa6, 0x52, 0x19, 0x7b, 0xae, 0x65, 0xe9, 0x86, 0x5f, 0x45, 0x33,
        0xae, 0xeb, 0x71, 0xd3, 0xfe, 0xa9, 0x7d, 0x26, 0xe0, 0xc9, 0xc7, 0xf2,
        0x2e, 0x4f, 0x95, 0xea, 0xb0, 0x83, 0x1d, 0xad, 0x77, 0xca, 0x32, 0x82,
        0x01, 0x21, 0x00, 0xc5, 0x47, 0x52, 0x67, 0xf1, 0x1b, 0x5c, 0x64, 0xd3,
        0xae, 0x61, 0x1a, 0x7d, 0x76, 0x0e, 0x7d, 0x23, 0x1e, 0x9c, 0xa0, 0x02,
        0x7c, 0x44, 0xa8, 0x6f, 0xf9, 0x27, 0x52, 0xd0, 0xa9, 0xc7, 0xd4, 0xbb,
        0x31, 0x69, 0x39, 0xfc, 0xb7, 0xa7, 0xcd, 0x86, 0xa1, 0x55, 0x71, 0x8a,
        0xb7, 0x48, 0x02, 0x73, 0x16, 0xf5, 0x9d, 0x83, 0x2f, 0xa8, 0x7e, 0x08,
        0x2f, 0x2d, 0xc4, 0xc3, 0xae, 0x6c, 0x65, 0x72, 0x42, 0xc1, 0x74, 0xd9,
        0x95, 0xd0, 0x10, 0xa1, 0x25, 0x9c, 0x5a, 0xf9, 0xa5, 0x08, 0x0d, 0xa2,
        0xed, 0x3a, 0xf1, 0xf7, 0x69, 0xc6, 0x29, 0xf4, 0x47, 0x43, 0x40, 0x7e,
        0x6d, 0x8b, 0xff, 0xae, 0x04, 0xf0, 0x09, 0x1d, 0x5f, 0x64, 0xd4, 0x7c,
        0xb8, 0x47, 0xf3, 0x84, 0xf2, 0x0f, 0x4a, 0x16, 0x32, 0xd1, 0xaf, 0x80,
        0x5a, 0xd6, 0x0b, 0xd8, 0xc5, 0xa4, 0x07, 0x0e, 0xbe, 0x52, 0x35, 0x99,
        0x9a, 0xa1, 0x4f, 0x44, 0x5d, 0xb7, 0x4d, 0x5d, 0xb1, 0xe2, 0xf9, 0x84,
        0x33, 0x6c, 0x51, 0xd6, 0xff, 0x27, 0x64, 0x86, 0x45, 0x56, 0xab, 0x73,
        0x1e, 0xb0, 0x21, 0x6e, 0x65, 0x19, 0x21, 0xf9, 0x97, 0x09, 0xb3, 0xec,
        0x32, 0x38, 0x79, 0x28, 0xe2, 0xd2, 0x03, 0x3a, 0x8d, 0xed, 0x8b, 0x34,
        0xa7, 0xf8, 0x0e, 0x26, 0x5b, 0x60, 0x20, 0x76, 0x43, 0x67, 0xe9, 0xea,
        0x3b, 0x0b, 0xbf, 0x25
    };

    byte key_[] = {
        0x25, 0x9f, 0xb6, 0xab, 0x17, 0xc4, 0xd8, 0x08, 0x07, 0x57, 0xa4, 0x90,
        0x57, 0x5a, 0x31, 0xff, 0x15, 0x98, 0xb4, 0x2e, 0x86, 0x4e, 0x7a, 0xcd,
        0xe3, 0x02, 0x58, 0x6f, 0xa8, 0x4b, 0x5c, 0xab, 0xcc, 0x91, 0xad, 0x8a,
        0xbd, 0xbe, 0x49, 0x51, 0x75, 0xed, 0x2d, 0xe9, 0x15, 0x8c, 0xfe, 0x19,
        0x68, 0x4a, 0xbf, 0x74, 0xf6, 0xe2, 0x60, 0xa7, 0xad, 0x89, 0x92, 0xdf,
        0x9d, 0x6d, 0x66, 0x4c
    };
#endif

    AssertIntEQ(0, wc_SrpInit(&cli, SRP_TYPE_SHA512, SRP_CLIENT_SIDE));
    AssertIntEQ(0, wc_SrpInit(&srv, SRP_TYPE_SHA512, SRP_SERVER_SIDE));

    AssertIntEQ(0, wc_SrpSetUsername(&cli, username_, usernameSz_));
    AssertIntEQ(0, wc_SrpSetUsername(&srv, username_, usernameSz_));

    AssertIntEQ(0, wc_SrpSetParams(&cli, N_,    sizeof(N_),
                                         g_,    sizeof(g_),
                                         salt_, sizeof(salt_)));
    AssertIntEQ(0, wc_SrpSetParams(&srv, N_,    sizeof(N_),
                                         g_,    sizeof(g_),
                                         salt_, sizeof(salt_)));

    AssertIntEQ(0, wc_SrpSetPassword(&cli, password_, passwordSz_));
    AssertIntEQ(0, wc_SrpSetVerifier(&srv, verifier_, sizeof(verifier_)));

    AssertIntEQ(0, wc_SrpSetPrivate(&cli, a_, sizeof(a_)));
    AssertIntEQ(0, wc_SrpGetPublic(&cli, clientPubKey, &clientPubKeySz));
    AssertIntEQ(0, XMEMCMP(clientPubKey, A_, clientPubKeySz));

    AssertIntEQ(0, wc_SrpSetPrivate(&srv, b_, sizeof(b_)));
    AssertIntEQ(0, wc_SrpGetPublic(&srv, serverPubKey, &serverPubKeySz));
    AssertIntEQ(0, XMEMCMP(serverPubKey, B_, serverPubKeySz));

    cli.keyGenFunc_cb = sha512_key_gen;
    AssertIntEQ(0, wc_SrpComputeKey(&cli, clientPubKey, clientPubKeySz,
                                          serverPubKey, serverPubKeySz));
    AssertIntEQ(0, XMEMCMP(cli.key, key_, sizeof(key_)));

    srv.keyGenFunc_cb = sha512_key_gen;
    AssertIntEQ(0, wc_SrpComputeKey(&srv, clientPubKey, clientPubKeySz,
                                          serverPubKey, serverPubKeySz));
    AssertIntEQ(0, XMEMCMP(srv.key, key_, sizeof(key_)));

    AssertIntEQ(0, wc_SrpGetProof(&cli, clientProof, &clientProofSz));
    AssertIntEQ(0, wc_SrpVerifyPeersProof(&srv, clientProof, clientProofSz));

    AssertIntEQ(0, wc_SrpGetProof(&srv, serverProof, &serverProofSz));
    AssertIntEQ(0, wc_SrpVerifyPeersProof(&cli, serverProof, serverProofSz));

    wc_SrpTerm(&cli);
    wc_SrpTerm(&srv);
}

#endif

void SrpTest(void)
{
#if defined(WOLFCRYPT_HAVE_SRP) && defined(WOLFSSL_SHA512)
    wolfCrypt_Init();
    test_SrpInit();
    test_SrpSetUsername();
    test_SrpSetParams();
    test_SrpSetPassword();
    test_SrpGetPublic();
    test_SrpComputeKey();
    test_SrpGetProofAndVerify();
    test_SrpKeyGenFunc_cb();
    wolfCrypt_Cleanup();
#endif
}
