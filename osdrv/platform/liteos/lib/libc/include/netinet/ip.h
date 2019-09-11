/*  $OpenBSD: ip.h,v 1.12 2006/04/27 02:19:32 tedu Exp $    */
/*  $NetBSD: ip.h,v 1.9 1995/05/15 01:22:44 cgd Exp $   */

/*
 * Copyright (c) 1982, 1986, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  @(#)ip.h    8.1 (Berkeley) 6/10/93
 */

#ifndef _NETINET_IP_H_
#define _NETINET_IP_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <endian.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#ifndef __LITEOS__
#include <linux/ip.h>
#else
#include <liteos/ip.h>
#endif

__BEGIN_DECLS

/*
 * Definitions for internet protocol version 4.
 * Per RFC 791, September 1981.
 */
#define IPVERSION   4

/*
 * Structure of an internet header, naked of options.
 */
struct ip {
#if BYTE_ORDER == LITTLE_ENDIAN
    u_int32_t ip_hl:4,      /* header length */
          ip_v:4;       /* version */
#endif
#if BYTE_ORDER == BIG_ENDIAN
    u_int32_t ip_v:4,       /* version */
          ip_hl:4;      /* header length */
#endif
    u_int8_t  ip_tos;       /* type of service */
    u_int16_t ip_len;       /* total length */
    u_int16_t ip_id;        /* identification */
    u_int16_t ip_off;       /* fragment offset field */
#define IP_RF 0x8000            /* reserved fragment flag */
#define IP_DF 0x4000            /* dont fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
    u_int8_t  ip_ttl;       /* time to live */
    u_int8_t  ip_p;         /* protocol */
    u_int16_t ip_sum;       /* checksum */
    struct    in_addr ip_src, ip_dst; /* source and dest address */
};

#define IP_MAXPACKET    65535       /* maximum packet size */

/* ECN RFC3168 obsoletes RFC2481, and these will be deprecated soon. */
#define IPTOS_CE        0x01    /* congestion experienced */
#define IPTOS_ECT       0x02    /* ECN-capable transport */


/*
 * ECN (Explicit Congestion Notification) codepoints in RFC3168
 * mapped to the lower 2 bits of the TOS field.
 */
#define IPTOS_ECN_NOTECT    0x00    /* not-ECT */
#define IPTOS_ECN_ECT1      0x01    /* ECN-capable transport (1) */
#define IPTOS_ECN_ECT0      0x02    /* ECN-capable transport (0) */
#define IPTOS_ECN_CE        0x03    /* congestion experienced */
#define IPTOS_ECN_MASK      0x03    /* ECN field mask */


#define IPOPT_DEBMEAS       0x40



#define IPOPT_SECURITY      130     /* provide s,c,h,tcc */

#define IPOPT_SATID     136     /* satnet id */

/*
 * Time stamp option structure.
 */
struct  ip_timestamp {
    u_int8_t ipt_code;      /* IPOPT_TS */
    u_int8_t ipt_len;       /* size of structure (variable) */
    u_int8_t ipt_ptr;       /* index of current entry */
#if _BYTE_ORDER == _LITTLE_ENDIAN
    u_int32_t ipt_flg:4,        /* flags, see below */
          ipt_oflw:4;       /* overflow counter */
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
    u_int32_t ipt_oflw:4,       /* overflow counter */
          ipt_flg:4;        /* flags, see below */
#endif
    union ipt_timestamp {
    n_time  ipt_time[1];
    struct  ipt_ta {
        struct in_addr ipt_addr;
        n_time ipt_time;
    } ipt_ta[1];
    } ipt_timestamp;
};

/* flag bits for ipt_flg */
#define IPOPT_TS_TSONLY     0       /* timestamps only */
#define IPOPT_TS_TSANDADDR  1       /* timestamps and addresses */
#define IPOPT_TS_PRESPEC    3       /* specified modules only */

/* bits for security (not byte swapped) */
#define IPOPT_SECUR_UNCLASS 0x0000
#define IPOPT_SECUR_CONFID  0xf135
#define IPOPT_SECUR_EFTO    0x789a
#define IPOPT_SECUR_MMMM    0xbc4d
#define IPOPT_SECUR_RESTR   0xaf13
#define IPOPT_SECUR_SECRET  0xd788
#define IPOPT_SECUR_TOPSECRET   0x6bc5

/*
 * Internet implementation parameters.
 */
#define MAXTTL      255     /* maximum time to live (seconds) */
#define IPDEFTTL    64      /* default ttl, from RFC 1340 */
#define IPFRAGTTL   60      /* time to live for frags, slowhz */
#define IPTTLDEC    1       /* subtracted when forwarding */

#define IP_MSS      576     /* default maximum segment size */

/*
 * This is the real IPv4 pseudo header, used for computing the TCP and UDP
 * checksums. For the Internet checksum, struct ipovly can be used instead.
 * For stronger checksums, the real thing must be used.
 */
struct ippseudo {
    struct    in_addr ippseudo_src; /* source internet address */
    struct    in_addr ippseudo_dst; /* destination internet address */
    u_int8_t  ippseudo_pad;     /* pad, must be zero */
    u_int8_t  ippseudo_p;       /* protocol */
    u_int16_t ippseudo_len;     /* protocol length */
};

/* BIONIC addition: declarations matching the Linux kernel */
/*                  some programs expect these...          */

#define IPOPT_OPTVAL 0
#define IPOPT_OLEN   1
#define IPOPT_OFFSET 2
__END_DECLS

#endif /* _NETINET_IP_H_ */
