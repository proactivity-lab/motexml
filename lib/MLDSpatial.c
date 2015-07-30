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
 * @brief Spatial functions.
 *
 * @author Raido Pahtma
 */
#include "MLDSpatial.h"
#include "MLD.h"

#ifndef __tinyos__
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#endif /* __tinyos__ */

	uint8_t MLDSpatial_getIntervalWithIndex(uint8_t ndex, uint8_t *buffer, uint8_t length, spatial_interval_t *interval) ATTR_TINYOS_AT_C {
		ml_object_t object;
		uint8_t locIndex = 0;
		int32_t value = 0;
		if(MLD_getObjectWithIndex(ndex,buffer,length,&object) == ndex) {
			interval->type = object.type;
			interval->subject = object.subject;
			switch(interval->type) {
				case dt_spiC:
					if(object.valueIsPresent) {
						interval->c.r = object.value;
					}
					else interval->c.r = 0; // It represents a point, value not needed
					if((MLD_findObjectWithParameters(dt_locZ, &ndex, NULL, buffer, length, &object) > 0)
						&& object.valueIsPresent) {
							interval->c.z = object.value;
					}
					else interval->c.z = 0; // z coordinate defaults to 0
					if(MLD_findObjectWithParameters(dt_locX, &ndex, NULL, buffer, length, &object) > 0) {
						if(object.valueIsPresent) {
							interval->c.x = object.value;
							if(MLD_findObjectWithParameters(dt_locY, &ndex, NULL, buffer, length, &object) > 0) {
								if(object.valueIsPresent) {
									interval->c.y = object.value;
									return ndex;
								}
							}
						}
					}
					break;
				case dt_spiA:
//					for(value = 1; value <= 4; value++) {
//						locIndex = MLD_findObjectWithParameters(dt_loc, &index, &value, buffer, length, &object);
//						if(locIndex > 0) {
//							if(MLD_findObjectWithParameters(dt_locX, &locIndex, NULL, buffer, length, &object)) {
//								if(object.valueIsPresent) {
//									interval->a.x[value - 1] = object.value;
//									if(MLD_findObjectWithParameters(dt_locY, &locIndex, NULL, buffer, length, &object)) {
//										if(object.valueIsPresent) interval->a.y[value - 1] = object.value;
//										else return 0; // Y value is missing
//									}
//									else return 0; // Y is missing
//								}
//								else return 0; // X value is missing
//							}
//							else return 0; // X is missing
//						}
//						else return 0; // Point is missing
//					}
//					return index;
					return 0;
				case dt_spiV:
					for(value = 1; value <= 2; value++) {
						locIndex = MLD_findObjectWithParameters(dt_location, &ndex, &value, buffer, length, &object);
						if(locIndex > 0) {
							if(MLD_findObjectWithParameters(dt_locX, &locIndex, NULL, buffer, length, &object)) {
								if(object.valueIsPresent) {
									interval->v.x[value - 1] = object.value;
									if(MLD_findObjectWithParameters(dt_locY, &locIndex, NULL, buffer, length, &object)) {
										if(object.valueIsPresent) interval->v.y[value - 1] = object.value;
										else return 0; // Y value is missing
									}
									else return 0; // Y is missing
								}
								else return 0; // X value is missing
							}
							else return 0; // X is missing
						}
						else return 0; // Point is missing
					}
					return ndex;
				default:
					break;
			}
		}
		return 0;
	}

	uint8_t MLDSpatial_findIntervalWithSubject(uint8_t subject, uint8_t *buffer, uint8_t length, spatial_interval_t *interval) ATTR_TINYOS_AT_C {
		ml_object_t object;
		uint8_t ndex = 0;
		ndex = MLD_findObjectWithParameters(dt_spiC, &subject, NULL, buffer, length, &object);
		if(ndex <= 0) ndex = MLD_findObjectWithParameters(dt_spiV, &subject, NULL, buffer, length, &object);
//		if(ndex <= 0) ndex = MLD_findObjectWithParameters(dt_spiA, &subject, NULL, buffer, length, &object);
		if(ndex > 0) return MLDSpatial_getIntervalWithIndex(ndex, buffer, length, interval);
		return 0;
	}

	int32_t MLDSpatial_distance(int32_t xc1, int32_t yc1, int32_t xc2, int32_t yc2) {
		int32_t xdiff = xc1 - xc2;
		int32_t ydiff = yc1 - yc2;
		return (int32_t)sqrt((double)(xdiff*xdiff + ydiff*ydiff));
	}

	bool MLDSpatial_intersectCircleLine(int32_t cx, int32_t cy, int32_t cr, int32_t lx1, int32_t ly1, int32_t lx2, int32_t ly2) ATTR_TINYOS_AT_C {
		// http://www.allegro.cc/forums/thread/589720/644831#target
		float A = cx - lx1;
		float B = cy - ly1;
		float C = lx2 - lx1;
		float D = ly2 - ly1;

		float dot = A * C + B * D;
		float len_sq = C * C + D * D;
		float param = dot / len_sq;

		float xx = 0;
		float yy = 0;

		if(param < 0) {
			xx = lx1;
			yy = ly1;
		}
		else if(param > 1) {
			xx = lx2;
			yy = ly2;
		}
		else {
			xx = lx1 + param * C;
			yy = ly1 + param * D;
		}
		if(MLDSpatial_distance(cx,cy,xx,yy) < cr) return TRUE;
		return FALSE;
	}

	void MLDSpatial_constructFourCorners(spatial_interval_t* intervalV, int32_t x[], int32_t y[]) ATTR_TINYOS_AT_C {
		if(intervalV->v.x[0] <= intervalV->v.x[1]) {
			x[0] = intervalV->v.x[0];
			x[1] = intervalV->v.x[1];
			x[2] = intervalV->v.x[0];
			x[3] = intervalV->v.x[1];
		}
		else {
			x[0] = intervalV->v.x[1];
			x[1] = intervalV->v.x[0];
			x[2] = intervalV->v.x[1];
			x[3] = intervalV->v.x[0];
		}
		if(intervalV->v.y[0] <= intervalV->v.y[1]) {
			y[0] = intervalV->v.y[0];
			y[1] = intervalV->v.y[0];
			y[2] = intervalV->v.y[1];
			y[3] = intervalV->v.y[1];
		}
		else {
			y[0] = intervalV->v.y[1];
			y[1] = intervalV->v.y[1];
			y[2] = intervalV->v.y[0];
			y[3] = intervalV->v.y[0];
		}
	}

	bool MLDSpatial_pointInsideParallelRectangle(int32_t x, int32_t y, int32_t rx[], int32_t ry[]) ATTR_TINYOS_AT_C {
		// Rectangle parallel to axis
		if((x >= rx[0]) && (x <= rx[3]) && (y >= ry[0]) && (y <= ry[3])) return TRUE;
		return FALSE;
	}

	bool MLDSpatial_linesIntersect(int32_t lx1, int32_t ly1, int32_t lx2, int32_t ly2, int32_t lx3, int32_t ly3, int32_t lx4, int32_t ly4) ATTR_TINYOS_AT_C {
		int32_t uat = (lx4 - lx3)*(ly1 - ly3) - (ly4 - ly3)*(lx1 - lx3);
		int32_t ubt = (lx2 - lx1)*(ly1 - ly3) - (ly2 - ly1)*(lx1 - lx3);
		int32_t u_b = (ly4 - ly3)*(lx2 - lx1) - (lx4 - lx3)*(ly2 - ly1);
		float ua = (float)uat / u_b;
		float ub = (float)ubt / u_b;

		if((ua > 0) && (ua < 1) && (ub > 0) && (ub < 1)) return TRUE;
		return FALSE;
	}

	bool MLDSpatial_overlapSpiASpiC(spatial_interval_t *intervalA, spatial_interval_t *intervalC) ATTR_TINYOS_AT_C {
		// TODO
#ifndef __tinyos__
		printf("Warning: spiA overlap functions not implemented and always return FALSE!\n");
#endif
		return FALSE;
	}

	bool MLDSpatial_overlapSpiA(spatial_interval_t *interval1, spatial_interval_t *interval2) ATTR_TINYOS_AT_C {
		// TODO
#ifndef __tinyos__
		printf("Warning: spiA overlap functions not implemented and always return FALSE!\n");
#endif
		return FALSE;
	}

	bool MLDSpatial_overlapSpiASpiV(spatial_interval_t *intervalA, spatial_interval_t *intervalV) ATTR_TINYOS_AT_C {
		// TODO
#ifndef __tinyos__
		printf("Warning: spiA overlap functions not implemented and always return FALSE!\n");
#endif
		return FALSE;
	}

	bool MLDSpatial_overlapSpiCSpiV(spatial_interval_t *intervalC, spatial_interval_t *intervalV) ATTR_TINYOS_AT_C {
		int32_t x[4];
		int32_t y[4];

		// It would be better if the algorithm was not dependant on the order of the points
		MLDSpatial_constructFourCorners(intervalV, x, y);

		// Check if any of the lines of the rectangle intersect with the circle
		if(MLDSpatial_intersectCircleLine(intervalC->c.x, intervalC->c.y, intervalC->c.r, x[0], y[0], x[1], y[1])) return TRUE;
		if(MLDSpatial_intersectCircleLine(intervalC->c.x, intervalC->c.y, intervalC->c.r, x[0], y[0], x[2], y[2])) return TRUE;
		if(MLDSpatial_intersectCircleLine(intervalC->c.x, intervalC->c.y, intervalC->c.r, x[3], y[3], x[1], y[1])) return TRUE;
		if(MLDSpatial_intersectCircleLine(intervalC->c.x, intervalC->c.y, intervalC->c.r, x[3], y[3], x[2], y[2])) return TRUE;

		// Check if the circle is completely inside the rectangle
		// Check the center of the circle against the rectangle - easy because it is parallel to axis
		if(MLDSpatial_pointInsideParallelRectangle(intervalC->c.x, intervalC->c.y, x, y)) return TRUE;

		// Check if the rectangle is completely inside the circle
		// Checking one point of the rectangle is enough
		if(MLDSpatial_distance(x[0], y[0], intervalC->c.x, intervalC->c.y) < intervalC->c.r) return TRUE;

		return FALSE;
	}

	bool MLDSpatial_overlapSpiC(spatial_interval_t *interval1, spatial_interval_t *interval2) ATTR_TINYOS_AT_C {
		int32_t d = MLDSpatial_distance(interval1->c.x, interval1->c.y, interval2->c.x, interval2->c.y);
		if(d < interval1->c.r + interval2->c.r) return TRUE;
		return FALSE;
	}

	bool MLDSpatial_overlapSpiV(spatial_interval_t *interval1, spatial_interval_t *interval2) ATTR_TINYOS_AT_C {
		int32_t sx1[4];
		int32_t sy1[4];
		int32_t sx2[4];
		int32_t sy2[4];

		MLDSpatial_constructFourCorners(interval1, sx1, sy1);
		MLDSpatial_constructFourCorners(interval2, sx2, sy2);

		if(MLDSpatial_linesIntersect(sx1[0], sy1[0], sx1[1], sy1[1], sx2[0], sy2[0], sx2[2], sy2[2])) return TRUE;
		if(MLDSpatial_linesIntersect(sx1[0], sy1[0], sx1[1], sy1[1], sx2[1], sy2[1], sx2[3], sy2[3])) return TRUE;
		if(MLDSpatial_linesIntersect(sx1[2], sy1[2], sx1[3], sy1[3], sx2[0], sy2[0], sx2[2], sy2[2])) return TRUE;
		if(MLDSpatial_linesIntersect(sx1[2], sy1[2], sx1[3], sy1[3], sx2[1], sy2[1], sx2[3], sy2[3])) return TRUE;

		if(MLDSpatial_linesIntersect(sx1[0], sy1[0], sx1[2], sy1[2], sx2[0], sy2[0], sx2[1], sy2[1])) return TRUE;
		if(MLDSpatial_linesIntersect(sx1[0], sy1[0], sx1[2], sy1[2], sx2[2], sy2[2], sx2[3], sy2[3])) return TRUE;
		if(MLDSpatial_linesIntersect(sx1[1], sy1[1], sx1[3], sy1[3], sx2[0], sy2[0], sx2[1], sy2[1])) return TRUE;
		if(MLDSpatial_linesIntersect(sx1[1], sy1[1], sx1[3], sy1[3], sx2[2], sy2[2], sx2[3], sy2[3])) return TRUE;

		// Check if rectangle 1 is inside rectangle 2, one point is enough
		if(MLDSpatial_pointInsideParallelRectangle(sx1[0], sy1[0], sx2, sy2)) return TRUE;

		// Check if rectangle 2 is inside rectangle 1, one point is enough
		if(MLDSpatial_pointInsideParallelRectangle(sx2[0], sy2[0], sx1, sy1)) return TRUE;

		return FALSE;
	}

	bool MLDSpatial_overlap(spatial_interval_t* interval1, spatial_interval_t* interval2) ATTR_TINYOS_AT_C {
		if((interval1 != NULL) && (interval2 != NULL)) {
			if(interval1->type == dt_spiC) {
				if(interval2->type == dt_spiC) return MLDSpatial_overlapSpiC(interval1, interval2);
				if(interval2->type == dt_spiA) return MLDSpatial_overlapSpiASpiC(interval2, interval1);
				if(interval2->type == dt_spiV) return MLDSpatial_overlapSpiCSpiV(interval1, interval2);
			}
			if(interval1->type == dt_spiA) {
				if(interval2->type == dt_spiA) return MLDSpatial_overlapSpiA(interval1, interval2);
				if(interval2->type == dt_spiC) return MLDSpatial_overlapSpiASpiC(interval1, interval2);
				if(interval2->type == dt_spiV) return MLDSpatial_overlapSpiASpiV(interval1, interval2);
			}
			if(interval1->type == dt_spiV) {
				if(interval2->type == dt_spiV) return MLDSpatial_overlapSpiV(interval1, interval2);
				if(interval2->type == dt_spiC) return MLDSpatial_overlapSpiCSpiV(interval2, interval1);
				if(interval2->type == dt_spiA) return MLDSpatial_overlapSpiASpiV(interval2, interval1);
			}
		}
		return FALSE;
	}
