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
#include "MLCompare.h"
#include "MLEncode.h"
#include "MLDecode.h"
#include "TemporalInterval.h"
#include "SpatialInterval.h"
#include "ML.h"
#include <stdio.h>


	int prepareEntry(uint8_t buf[], uint8_t length, int32_t id, int32_t type, int32_t value, int32_t start, int32_t time, int32_t x, int32_t y, int32_t r) {
		MLEncode enc;
		int32_t index;
		spatial_interval_t spint;
		temporal_interval_t tint;

		enc.initialize(buf, length);

		index = enc.appendObjectWithValues(dt_aId, NULL, &id);

		index = enc.appendObjectWithValues(type, &index, &value);

		spint.subject = index;
		spint.subjectIsPresent = TRUE;
		spint.type = dt_spiC;
		spint.c.x = x;
		spint.c.y = y;
		spint.c.r = r;
		enc.appendSpatialInterval(&spint);

		tint.subject = index;
		tint.subjectIsPresent = TRUE;
		tint.type = dt_timeAbs;
		tint.start = start;
		tint.intTime = time;
		enc.appendTemporalInterval(&tint);

		return enc.finalize();
	}

	int main() {

		MLCompare cmp(10, 3, 3);

		uint8_t entry[10][200];
		uint8_t lengths[10];

		uint8_t conditions[10][200];
		uint8_t clengths[10];

		printf("Entries... \n");

		lengths[0] = prepareEntry(entry[0], 200, 1, dt_tmpC, 25, 1000, 60, 100, 100, 75);
		lengths[1] = prepareEntry(entry[1], 200, 2, dt_tmpC, 24, 1024, 60, 200, 200, 50);
		lengths[2] = prepareEntry(entry[2], 200, 1, dt_tmpC, 26, 2000, 60, 100, 100, 75);
		lengths[3] = prepareEntry(entry[3], 200, 2, dt_tmpC, 23, 2045, 60, 200, 200, 50);
		lengths[4] = prepareEntry(entry[4], 200, 1, dt_tmpC, 25, 3000, 60, 100, 100, 75);
		lengths[5] = prepareEntry(entry[5], 200, 2, dt_tmpC, 25, 3500, 60, 200, 200, 50);
		lengths[6] = prepareEntry(entry[6], 200, 3, dt_hum, 75, 4136, 60, 230, 200, 50);
		lengths[7] = prepareEntry(entry[7], 200, 3, dt_hum, 80, 4256, 60, 230, 200, 50);
		lengths[8] = prepareEntry(entry[8], 200, 3, dt_tmpC, 37, 4135, 60, 230, 200, 50);
		lengths[9] = prepareEntry(entry[9], 200, 3, dt_tmpF, 100, 4136, 60, 230, 200, 50);

		printf("Conditions... \n");

		MLEncode enc;
		enc.initialize(conditions[0], 200);
		int32_t index = enc.appendObjectWithValues(dt_tmpC, NULL, NULL);
		int32_t value = 30;
		enc.appendObjectWithValues(dt_less, &index, &value); // Get temps celsius under 30C
		clengths[0] = enc.finalize();

		enc.initialize(conditions[1], 200);
		index = enc.appendObjectWithValues(dt_tmpC, NULL, NULL);
		value = 23;
		enc.appendObjectWithValues(dt_greater, &index, &value);
		value = 26;
		enc.appendObjectWithValues(dt_less, &index, &value);
		clengths[1] = enc.finalize();

		enc.initialize(conditions[2], 200);
		index = enc.appendObjectWithValues(dt_tmpC, NULL, NULL);
		spatial_interval_t spint;
		spint.subjectIsPresent = TRUE;
		spint.subject = index;
		spint.type = dt_spiC;
		spint.c.r = 200;
		spint.c.x = 0;
		spint.c.y = 0;
		enc.appendSpatialInterval(&spint);
		clengths[2] = enc.finalize();

		enc.initialize(conditions[3], 200);
		index = enc.appendObjectWithValues(dt_tmpC, NULL, NULL);
		temporal_interval_t tint;
		tint.subjectIsPresent = TRUE;
		tint.subject = index;
		tint.type = dt_timeAbs;
		tint.start = 2000;
		tint.intTime = 2000;
		enc.appendTemporalInterval(&tint);
		clengths[3] = enc.finalize();

		enc.initialize(conditions[4], 200);
		tint.subjectIsPresent = FALSE;
		tint.type = dt_timeAbs;
		tint.start = 2000;
		tint.intTime = 2000;
		enc.appendTemporalInterval(&tint);
		clengths[4] = enc.finalize();

		for(int i=0;i<10;i++) {
			if(cmp.initialize(conditions[i], clengths[i]) == SUCCESS) {
				printf("Initialized condition %d.\n", i);
				cmp.printConditions();
				for(int j=0;j<10;j++) {
					if(cmp.satisfies(entry[j], lengths[j])) {
							printf("Item %d matches condition %d\n", j, i);
					}
					else printf("Item %d does not match condition %d\n", j, i);
				}
				printf("\n");
			}
			else printf("Initializing condition %d failed!\n", i);
		}

		return 0;
	}
