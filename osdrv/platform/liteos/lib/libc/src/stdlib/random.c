
/*    $NetBSD: random.c,v 1.4 2014/06/12 20:59:46 christos Exp $    */

/*
 * Copyright (c) 1983, 1993
 *    The Regents of the University of California.  All rights reserved.
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
 */

 /*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "stdlib.h"
#include "reentrant.h"

#ifndef SMALL_RANDOM
static void srandom_unlocked(unsigned int);
static long random_unlocked(void);

#define USE_BETTER_RANDOM

/*
 * random.c:
 *
 * An improved random number generation package.  In addition to the standard
 * rand()/srand() like interface, this package also has a special state info
 * interface.  The initstate() routine is called with a seed, an array of
 * bytes, and a count of how many bytes are being passed in; this array is
 * then initialized to contain information for random number generation with
 * that much state information.  Good sizes for the amount of state
 * information are 32, 64, 128, and 256 bytes.  The state can be switched by
 * calling the setstate() routine with the same array as was initiallized
 * with initstate().  By default, the package runs with 128 bytes of state
 * information and generates far better random numbers than a linear
 * congruential generator.  If the amount of state information is less than
 * 32 bytes, a simple linear congruential R.N.G. is used.
 *
 * Internally, the state information is treated as an array of ints; the
 * zeroeth element of the array is the type of R.N.G. being used (small
 * integer); the remainder of the array is the state information for the
 * R.N.G.  Thus, 32 bytes of state information will give 7 ints worth of
 * state information, which will allow a degree seven polynomial.  (Note:
 * the zeroeth word of state information also has some other information
 * stored in it -- see setstate() for details).
 *
 * The random number generation technique is a linear feedback shift register
 * approach, employing trinomials (since there are fewer terms to sum up that
 * way).  In this approach, the least significant bit of all the numbers in
 * the state table will act as a linear feedback shift register, and will
 * have period 2^deg - 1 (where deg is the degree of the polynomial being
 * used, assuming that the polynomial is irreducible and primitive).  The
 * higher order bits will have longer periods, since their values are also
 * influenced by pseudo-random carries out of the lower bits.  The total
 * period of the generator is approximately deg*(2**deg - 1); thus doubling
 * the amount of state information has a vast influence on the period of the
 * generator.  Note: the deg*(2**deg - 1) is an approximation only good for
 * large deg, when the period of the shift register is the dominant factor.
 * With deg equal to seven, the period is actually much longer than the
 * 7*(2**7 - 1) predicted by this formula.
 *
 * Modified 28 December 1994 by Jacob S. Rosenberg.
 * The following changes have been made:
 * All references to the type u_int have been changed to unsigned long.
 * All references to type int have been changed to type long.  Other
 * cleanups have been made as well.  A warning for both initstate and
 * setstate has been inserted to the effect that on Sparc platforms
 * the 'arg_state' variable must be forced to begin on word boundaries.
 * This can be easily done by casting a long integer array to char *.
 * The overall logic has been left STRICTLY alone.  This software was
 * tested on both a VAX and Sun SpacsStation with exactly the same
 * results.  The new version and the original give IDENTICAL results.
 * The new version is somewhat faster than the original.  As the
 * documentation says:  "By default, the package runs with 128 bytes of
 * state information and generates far better random numbers than a linear
 * congruential generator.  If the amount of state information is less than
 * 32 bytes, a simple linear congruential R.N.G. is used."  For a buffer of
 * 128 bytes, this new version runs about 19 percent faster and for a 16
 * byte buffer it is about 5 percent faster.
 *
 * Modified 07 January 2002 by Jason R. Thorpe.
 * The following changes have been made:
 * All the references to "long" have been changed back to "int".  This
 * fixes memory corruption problems on LP64 platforms.
 */

/*
 * For each of the currently supported random number generators, we have a
 * break value on the amount of state information (you need at least this
 * many bytes of state info to support this random number generator), a degree
 * for the polynomial (actually a trinomial) that the R.N.G. is based on, and
 * the separation between the two lower order coefficients of the trinomial.
 */
#define    TYPE_0        0        /* linear congruential */
#define    BREAK_0        8
#define    DEG_0        0
#define    SEP_0        0

#define    TYPE_1        1        /* x**7 + x**3 + 1 */
#define    BREAK_1        32
#define    DEG_1        7
#define    SEP_1        3

#define    TYPE_2        2        /* x**15 + x + 1 */
#define    BREAK_2        64
#define    DEG_2        15
#define    SEP_2        1

#define    TYPE_3        3        /* x**31 + x**3 + 1 */
#define    BREAK_3        128
#define    DEG_3        31
#define    SEP_3        3

#define    TYPE_4        4        /* x**63 + x + 1 */
#define    BREAK_4        256
#define    DEG_4        63
#define    SEP_4        1

/*
 * Array versions of the above information to make code run faster --
 * relies on fact that TYPE_i == i.
 */
#define    MAX_TYPES    5        /* max number of types above */

/*
 * Initially, everything is set up as if from:
 *
 *    initstate(1, &randtbl, 128);
 *
 * Note that this initialization takes advantage of the fact that srandom()
 * advances the front and rear pointers 10*rand_deg times, and hence the
 * rear pointer which starts at 0 will also end up at zero; thus the zeroeth
 * element of the state information, which contains info about the current
 * position of the rear pointer is just
 *
 *    MAX_TYPES * (rptr - state) + TYPE_3 == TYPE_3.
 */

/* LINTED */
static int randtbl[DEG_3 + 1] = { /*lint -e569*/
    TYPE_3,
#ifdef USE_BETTER_RANDOM
    0x991539b1, 0x16a5bce3, 0x6774a4cd,
    0x3e01511e, 0x4e508aaa, 0x61048c05,
    0xf5500617, 0x846b7115, 0x6a19892c,
    0x896a97af, 0xdb48f936, 0x14898454,
    0x37ffd106, 0xb58bff9c, 0x59e17104,
    0xcf918a49, 0x09378c83, 0x52c7a471,
    0x8d293ea9, 0x1f4fc301, 0xc3db71be,
    0x39b44e1c, 0xf8a44ef9, 0x4c8b80b1,
    0x19edc328, 0x87bf4bdd, 0xc9b240e5,
    0xe9ee4b1b, 0x4382aee7, 0x535b6b41,
    0xf3bec5da,
#else
    0x9a319039, 0x32d9c024, 0x9b663182,
    0x5da1f342, 0xde3b81e0, 0xdf0a6fb5,
    0xf103bc02, 0x48f340fb, 0x7449e56b,
    0xbeb1dbb0, 0xab5c5918, 0x946554fd,
    0x8c2e680f, 0xeb3d799f, 0xb11ee0b7,
    0x2d436b86, 0xda672e2a, 0x1588ca88,
    0xe369735d, 0x904f35f7, 0xd7158fd6,
    0x6fa6f051, 0x616e6b96, 0xac94efdc,
    0x36413f93, 0xc622c298, 0xf5a42ab8,
    0x8a88d77b, 0xf5ad9d0e, 0x8999220b,
    0x27fb47b9,
#endif /* USE_BETTER_RANDOM */
};/*lint +e569*/

/*
 * fptr and rptr are two pointers into the state info, a front and a rear
 * pointer.  These two pointers are always rand_sep places aparts, as they
 * cycle cyclically through the state information.  (Yes, this does mean we
 * could get away with just one pointer, but the code for random() is more
 * efficient this way).  The pointers are left positioned as they would be
 * from the call
 *
 *    initstate(1, randtbl, 128);
 *
 * (The position of the rear pointer, rptr, is really 0 (as explained above
 * in the initialization of randtbl) because the state table pointer is set
 * to point to randtbl[1] (as explained below).
 */
static int *fptr = &randtbl[SEP_3 + 1];
static int *rptr = &randtbl[1];

/*
 * The following things are the pointer to the state information table, the
 * type of the current generator, the degree of the current polynomial being
 * used, and the separation between the two pointers.  Note that for efficiency
 * of random(), we remember the first location of the state information, not
 * the zeroeth.  Hence it is valid to access state[-1], which is used to
 * store the type of the R.N.G.  Also, we remember the last location, since
 * this is more efficient than indexing every time to find the address of
 * the last element to see if the front and rear pointers have wrapped.
 */
static int *state = &randtbl[1];
static int rand_type = TYPE_3;
static int rand_deg = DEG_3;
static int rand_sep = SEP_3;
static int *end_ptr = &randtbl[DEG_3 + 1];

/*
 * srandom:
 *
 * Initialize the random number generator based on the given seed.  If the
 * type is the trivial no-state-information type, just remember the seed.
 * Otherwise, initializes state[] based on the given "seed" via a linear
 * congruential generator.  Then, the pointers are set to known locations
 * that are exactly rand_sep places apart.  Lastly, it cycles the state
 * information a given number of times to get rid of any initial dependencies
 * introduced by the L.C.R.N.G.  Note that the initialization of randtbl[]
 * for default usage relies on values produced by this routine.
 */
static void
srandom_unlocked(unsigned int x)
{
    int i;

    if (rand_type == TYPE_0)
        state[0] = x;
    else {
        state[0] = x;
        for (i = 1; i < rand_deg; i++) {
#ifdef USE_BETTER_RANDOM
            int x1, hi, lo, t;

            /*
             * Compute x[n + 1] = (7^5 * x[n]) mod (2^31 - 1).
             * From "Random number generators: good ones are hard
             * to find", Park and Miller, Communications of the ACM,
             * vol. 31, no. 10,
             * October 1988, p. 1195.
             */
            x1 = state[i - 1];
            hi = x1 / 127773;
            lo = x1 % 127773;
            t = 16807 * lo - 2836 * hi;
            if (t <= 0)
                t += 0x7fffffff;
            state[i] = t;
#else
            state[i] = 1103515245 * state[i - 1] + 12345;
#endif /* USE_BETTER_RANDOM */
        }
        fptr = &state[rand_sep];
        rptr = &state[0];
        for (i = 0; i < 10 * rand_deg; i++)
            (void)random_unlocked();
    }
}

void
srandom(unsigned int x)
{
      UINTPTR intSave;
      /*
        Here we use LOS_IntLock instead of  (void)mutex_lock(&random_mutex),
        for that mutex_lock() is a blocking API, and srandom() may be used in software timer callback.
        Using blocking API in software timer callback is not allowed.
      */
      intSave = LOS_IntLock();
      srandom_unlocked(x);
      LOS_IntRestore(intSave);
}

/*
 * random:
 *
 * If we are using the trivial TYPE_0 R.N.G., just do the old linear
 * congruential bit.  Otherwise, we do our fancy trinomial stuff, which is
 * the same in all the other cases due to all the global variables that have
 * been set up.  The basic operation is to add the number at the rear pointer
 * into the one at the front pointer.  Then both pointers are advanced to
 * the next location cyclically in the table.  The value returned is the sum
 * generated, reduced to 31 bits by throwing away the "least random" low bit.
 *
 * Note: the code takes advantage of the fact that both the front and
 * rear pointers can't wrap on the same call by not testing the rear
 * pointer if the front one has wrapped.
 *
 * Returns a 31-bit random number.
 */
static long
random_unlocked(void)
{
    int i;
    int *f, *r;

    if (rand_type == TYPE_0) {
        i = state[0];
        state[0] = i = (i * 1103515245 + 12345) & 0x7fffffff;
    } else {
        /*
         * Use local variables rather than static variables for speed.
         */
        f = fptr; r = rptr;
        *f += *r;
        /* chucking least random bit */
        i = ((unsigned int)*f >> 1) & 0x7fffffff;
        if (++f >= end_ptr) {
            f = state;
            ++r;
        }
        else if (++r >= end_ptr) {
            r = state;
        }

        fptr = f; rptr = r;
    }
    return(i);
}

long
random(void)
{
    long r;
      UINTPTR intSave;
      /*
        Here we use LOS_IntLock instead of  (void)mutex_lock(&random_mutex),
        for that mutex_lock() is a blocking API, and random() may be used in software timer callback.
        Using blocking API in software timer callback is not allowed.
      */
      intSave = LOS_IntLock();
    r = random_unlocked();
      LOS_IntRestore(intSave);
    return (r);
}
#else
long
random(void)
{
    static u_long randseed = 1;
    long x, hi, lo, t;

    /*
     * Compute x[n + 1] = (7^5 * x[n]) mod (2^31 - 1).
     * From "Random number generators: good ones are hard to find",
     * Park and Miller, Communications of the ACM, vol. 31, no. 10,
     * October 1988, p. 1195.
     */
    x = randseed;
    hi = x / 127773;
    lo = x % 127773;
    t = 16807 * lo - 2836 * hi;
    if (t <= 0)
        t += 0x7fffffff;
    randseed = t;
    return (t);
}
#endif /* SMALL_RANDOM */



int rand() {
  return random();
}

void srand(unsigned int seed) {
   srandom(seed);
   return;
}

