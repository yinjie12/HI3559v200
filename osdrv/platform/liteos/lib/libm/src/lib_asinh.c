/* @(#)s_asinh.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#include "math.h"
#include "float.h"

static const double one =  1.00000000000000000000e+00; /* 0x3FF00000, 0x00000000 */
static const double ln2 =  6.93147180559945286227e-01; /* 0x3FE62E42, 0xFEFA39EF */
static const double huges=  1.00000000000000000000e+300;

double
asinh(double x)
{
    double t,w;
    int hx,ix;
    GET_HIGH_WORD(hx,x);
    ix = hx&0x7fffffff;
    if(ix>=0x7ff00000) return x+x;    /* x is inf or NaN */
    if(ix< 0x3e300000) {    /* |x|<2**-28 */
        if((huges+x)>one) return x;    /* return x inexact except 0 */
    }
    if(ix>0x41b00000) {    /* |x| > 2**28 */
        w = __ieee754_log(fabs(x))+ln2;
    } else if (ix>0x40000000) {    /* 2**28 > |x| > 2.0 */
        t = fabs(x);
        w = __ieee754_log(2.0*t+one/(sqrt(x*x+one)+t));
    } else {        /* 2.0 > |x| > 2**-28 */
        t = x*x;
        w =log1p(fabs(x)+t/(one+sqrt(one+t)));
    }
    if(hx>0) return w; else return -w;
}
