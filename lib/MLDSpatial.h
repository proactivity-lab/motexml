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
 * @brief Spatial function headers.
 *
 * @author Raido Pahtma
 */
#ifndef MLDSPATIAL_H_
#define MLDSPATIAL_H_
#include "ML.h"

	uint8_t MLDSpatial_getIntervalWithIndex(uint8_t ndex, uint8_t *buffer, uint8_t length, spatial_interval_t *interval) ATTR_TINYOS_AT_C;
	uint8_t MLDSpatial_findIntervalWithSubject(uint8_t subject, uint8_t *buffer, uint8_t length, spatial_interval_t *interval) ATTR_TINYOS_AT_C;

	bool MLDSpatial_overlap(spatial_interval_t* interval1, spatial_interval_t* interval2) ATTR_TINYOS_AT_C;

	int32_t MLDSpatial_distance(int32_t px1, int32_t py1, int32_t px2, int32_t py2);

	bool MLDSpatial_intersectCircleLine(int32_t cx, int32_t cy, int32_t cr, int32_t lx1, int32_t ly1, int32_t lx2, int32_t ly2) ATTR_TINYOS_AT_C;
	void MLDSpatial_constructFourCorners(spatial_interval_t* intervalV, int32_t x[], int32_t y[]) ATTR_TINYOS_AT_C;

	bool MLDSpatial_pointInsideParallelRectangle(int32_t x, int32_t y, int32_t rx[], int32_t ry[]) ATTR_TINYOS_AT_C;
	bool MLDSpatial_linesIntersect(int32_t lx1, int32_t ly1, int32_t lx2, int32_t ly2, int32_t lx3, int32_t ly3, int32_t lx4, int32_t ly4) ATTR_TINYOS_AT_C;
	bool MLDSpatial_overlapSpiASpiC(spatial_interval_t *intervalA, spatial_interval_t *intervalC) ATTR_TINYOS_AT_C;
	bool MLDSpatial_overlapSpiA(spatial_interval_t *interval1, spatial_interval_t *interval2) ATTR_TINYOS_AT_C;
	bool MLDSpatial_overlapSpiASpiV(spatial_interval_t *intervalA, spatial_interval_t *intervalV) ATTR_TINYOS_AT_C;
	bool MLDSpatial_overlapSpiCSpiV(spatial_interval_t *intervalC, spatial_interval_t *intervalV) ATTR_TINYOS_AT_C;
	bool MLDSpatial_overlapSpiC(spatial_interval_t *interval1, spatial_interval_t *interval2) ATTR_TINYOS_AT_C;
	bool MLDSpatial_overlapSpiV(spatial_interval_t *interval1, spatial_interval_t *interval2) ATTR_TINYOS_AT_C;

#endif /* MLDSPATIAL_H_ */
