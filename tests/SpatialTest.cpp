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
#include "SpatialInterval.h"
#include <cstdio>

	void testOverlap(int32_t cx, int32_t cy, int32_t cr, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
		spatial_interval_t cspint;
		spatial_interval_t vspint;

		cspint.type = dt_spiC;
		vspint.type = dt_spiV;

		cspint.c.x = cx;
		cspint.c.y = cy;
		cspint.c.r = cr;

		vspint.v.x[0] = x1;
		vspint.v.y[0] = y1;
		vspint.v.x[1] = x2;
		vspint.v.y[1] = y2;

		if(SpatialInterval::overlap(&cspint, &vspint)) printf("Overlap\n");
		else printf("Don't Overlap\n");
	}

	void testOverlapV(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
		spatial_interval_t vspint1;
		spatial_interval_t vspint2;

		vspint1.type = dt_spiV;
		vspint2.type = dt_spiV;

		vspint1.v.x[0] = 100;
		vspint1.v.y[0] = 200;
		vspint1.v.x[1] = 300;
		vspint1.v.y[1] = 400;

		vspint2.v.x[0] = x1;
		vspint2.v.y[0] = y1;
		vspint2.v.x[1] = x2;
		vspint2.v.y[1] = y2;

		if(SpatialInterval::overlap(&vspint1, &vspint2)) printf("Overlap\n");
		else printf("Don't Overlap\n");
	}

	int main() {
		testOverlap(200, 200, 50, 100, 100, 200, 200); // Right upper corner
		testOverlap(200, 200, 50, 180, 180, 220, 220); // Inside the circle
		testOverlap(200, 200, 50, 250, 250, 300, 300); // Should not overlap
		testOverlap(200, 200, 50, 150, 150, 180, 180); // Right upper corner
		testOverlap(200, 200, 50, 100, 220, 400, 600); // Lower line
		testOverlap(200, 200, 50, 100, 220, 400, 225); // Lower and upper lines
		testOverlap(200, 200, 50, 100, 100, 300, 300); // Square inside the circle
		printf("\n");
		testOverlapV(100, 100, 300, 300);
		testOverlapV(0, 0, 500, 500);
		testOverlapV(0, 0, 100, 100); // Should not overlap
		testOverlapV(50, 250, 500, 350);
		testOverlapV(150, 250, 0, 500);
		printf("\n");
		testOverlap(100, 300, 100, 289, 368, 322, 328);
		testOverlap(200, 300, 100, 289, 368, 322, 328);
	}
