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
 * @brief Temporal functions.
 *
 * @author Raido Pahtma
 */
#include "MLDTemporal.h"
#include "MLD.h"

#ifndef __tinyos__
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#endif /* __tinyos__ */

#ifndef __tinyos__

	uint32_t MLDTemporal_timeNow() ATTR_TINYOS_AT_C {
		return time(NULL);
	}

#endif /* __tinyos__ */

	uint8_t MLDTemporal_getIntervalWithIndex(uint8_t ndex, uint8_t buffer[], uint8_t length, temporal_interval_t* interval) ATTR_TINYOS_AT_C {
		ml_object_t object;
		if(MLD_getObjectWithIndex(ndex,buffer,length,&object) == ndex) {
			if(object.valueIsPresent) {
				interval->type = object.type;
				interval->subject = object.subject;
				switch(interval->type) {
					case dt_timeR:
						interval->elapsed = object.value;
						break;
					case dt_timeAbs:
						interval->start = (uint32_t)object.value;
						break;
					default: // Unknown temporal interval type
						return 0;
				}
				if(MLD_findObjectWithParameters(dt_intTime, &ndex, NULL, buffer, length, &object) > 0) { // Get duration
					if(object.valueIsPresent) {
						interval->intTime = object.value;
						return ndex;
					}
				}
			}
		}
		return 0;
	}

	uint8_t MLDTemporal_findIntervalWithSubject(uint8_t subject, uint8_t buffer[], uint8_t length, temporal_interval_t* interval) ATTR_TINYOS_AT_C {
		ml_object_t object;
		uint8_t ndex = 0;
		ndex = MLD_findObjectWithParameters(dt_timeR, &subject, NULL, buffer, length, &object); // Search for relative temporal interval
		if(ndex == 0) ndex = MLD_findObjectWithParameters(dt_timeAbs, &subject, NULL, buffer, length, &object); // If not found, search for absolute temporal interval
		if(ndex > 0) return MLDTemporal_getIntervalWithIndex(ndex, buffer, length, interval);
		return 0;
	}

	error_t MLDTemporal_changeToAbsolute(temporal_interval_t* interval) ATTR_TINYOS_AT_C {
		if(interval != NULL) {
			if(interval->type == dt_timeR) {
				interval->type = dt_timeAbs;
				interval->start = MLDTemporal_timeNow() - interval->elapsed;
				return SUCCESS;
			}
		}
		return FAIL;
	}

	error_t MLDTemporal_changeToRelative(temporal_interval_t* interval) ATTR_TINYOS_AT_C {
		if(interval != NULL) {
			if(interval->type == dt_timeAbs) {
				interval->type = dt_timeR;
				interval->elapsed = MLDTemporal_timeNow() - interval->start;
				return SUCCESS;
			}
		}
		return FAIL;
	}

	bool MLDTemporal_overlap(temporal_interval_t* interval1, temporal_interval_t* interval2) ATTR_TINYOS_AT_C {
		uint32_t start1 = 0;
		uint32_t start2 = 0;
		if((interval1 != NULL) && (interval2 != NULL)) {
			if(interval1->type == dt_timeR) start1 = MLDTemporal_timeNow() - interval1->elapsed; // Change to absolute
			else start1 = interval1->start;
			if(interval2->type == dt_timeR) start2 = MLDTemporal_timeNow() - interval2->elapsed; // Change to absolute
			else start2 = interval2->start;
			if((start1 >= start2) && (start1 <= (start2 + interval2->intTime))) return TRUE; // Interval1 starts in the middle of interval2
			if((start2 >= start1) && (start2 <= (start1 + interval1->intTime))) return TRUE; // Interval2 starts in the middle of interval1
		}
		return FALSE;
	}
