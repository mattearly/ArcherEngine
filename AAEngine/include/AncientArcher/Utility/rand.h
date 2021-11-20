/*
rand
----------------------------------------------------------------------
Copyright (c) 2019-2020, Matthew Early matthewjearly@gmail.com
All rights reserved.
Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the
following conditions are met:
* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.
* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.
* Neither the name of the Matthew Early, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission from Matthew Early.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----------------------------------------------------------------------
*/

/** @file rand.h
 *  @brief Definition of functions that produce random results.
 */

#pragma once
namespace AA {
/**
 * Returns a random float between zero and one.
 */
float ZTORf();

/**
 * Returns a random double between zero and one.
 */
double ZTOR();

/**
 * Returns a random integer zero or one.
 */
int ZOOR();

/**
 * Returns and random integer with the bounds n and k.
 */
int NTKR(int n, int k);

/**
 * Returns a random float with the bounds n and k.
 */
float NTKR(float n, float k);

/**
 * Returns a random double with the bounds n and k.
 */
double NTKR(double n, double k);


// SIMPLIFIED SINGLE DICE ROLLERS
// real rolls - dice replicants  (only 1d)
#define roll1d2   ZOOR() + 1
#define roll1d3   NTKR(1, 3)
#define roll1d4   NTKR(1, 4)
#define roll1d6   NTKR(1, 6)
#define roll1d8   NTKR(1, 8)
#define roll1d10  NTKR(1, 10)
#define roll1d12  NTKR(1, 12)
#define roll1d20  NTKR(1, 20)
#define roll1d30  NTKR(1, 30)
#define roll1d34  NTKR(1, 34)
#define roll1d40  NTKR(1, 40)
#define roll1d60  NTKR(1, 60)
#define roll1d100 NTKR(1, 100)
#define roll1d120 NTKR(1, 120)
#define roll1d400 NTKR(1, 400)

// further aliases
#define rolld2   roll1d2
#define rolld3   roll1d3
#define rolld4   roll1d4
#define rolld6   roll1d6
#define rolld8   roll1d8
#define rolld10  roll1d10
#define rolld12  roll1d12
#define rolld20  roll1d20
#define rolld30  roll1d30
#define rolld34  roll1d34
#define rolld40  roll1d40
#define rolld60  roll1d60
#define rolld100 roll1d100
#define rolld120 roll1d120
#define rolld400 roll1d400

// unatural rolls
#define roll1d1000      NTKR(1, 1000)      // 1 to 1,000   Thousand
#define roll1d10000     NTKR(1, 10000)     // 1 to 10,000  Ten Thousand
#define roll1d100000    NTKR(1, 100000)    // 1 to 100,000  Hundred Thousand
#define roll1d1000000   NTKR(1, 100000)    // 1 to 1,000,000  Million
#define roll1d10000000  NTKR(1, 1000000)   // 1 to 10,000,000  Ten Million
#define roll1d100000000 NTKR(1, 10000000)  // 1 to 100,000,000  Hundred Million

#define rolld1000      roll1d1000
#define rolld10000     roll1d10000
#define rolld100000    roll1d100000
#define rolld1000000   roll1d1000000
#define rolld10000000  roll1d10000000
#define rolld100000000 roll1d100000000

}  // end namespace AA