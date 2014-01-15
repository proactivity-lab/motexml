/*
 * Copyright (c) 2011 Tallinn University of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the copyright holders nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * @brief Test app.
 *
 * @author Raido Pahtma
 */
#include "MLEncode.h"
#include "MLDecode.h"
#include "TemporalInterval.h"
#include "SpatialInterval.h"
#include <cstdio>
#include <ctime>


	int main() {
		temporal_interval_t tint1;
		temporal_interval_t tint2;
		temporal_interval_t tint3;

		tint1.type = dt_timeAbs;
		tint1.intTime = 60;
		tint1.start = time(NULL) - 60;

		TemporalInterval::printTemporalInterval(&tint1);

		TemporalInterval::changeIntervalToRelative(&tint1);

		TemporalInterval::printTemporalInterval(&tint1);

		TemporalInterval::changeIntervalToAbsolute(&tint1);

		TemporalInterval::printTemporalInterval(&tint1);

		tint2.type = dt_timeR;
		tint2.intTime = 10;
		tint2.elapsed = 30;

		TemporalInterval::printTemporalInterval(&tint2);

		if(TemporalInterval::overlap(&tint1, &tint2)) printf("Intervals overlap.\n");

		tint3.type = dt_timeR;
		tint3.intTime = 30;
		tint3.elapsed = 91;

		TemporalInterval::printTemporalInterval(&tint3);

		if(TemporalInterval::overlap(&tint1, &tint3)) printf("Intervals overlap.\n");

		return 0;
	}
