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
 * @brief Comparison functions.
 *
 * @author Raido Pahtma
 */
#include "MLCompare.h"
#include "MLD.h"
#include "MLDTemporal.h"
#include "MLDSpatial.h"
#include "MLObjects.h"

#include <stdio.h>

	error_t MLCompare_initialize(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) {
		int32_t position = 1;
		ml_object_t object;

		cmp->condition_count = 0;
		cmp->spatial_count = 0;
		cmp->temporal_count = 0;

		while((MLD_getObjectWithIndex(position,buffer,length,&object)) > 0) {
			if(MLObjects_isSimpleConditionObject(object.type)) {
				cmp->conditions[cmp->condition_count].oper = object.type;
				// Find subject index
				if(object.subject > 0) {
					// Find comparison value
					if(object.type != dt_present) {
						if(object.valueIsPresent) {
							cmp->conditions[cmp->condition_count].object_value = object.value;
							// Find multiplier or divider for the value - may not be present
							//TODO exponent
						}
						else return FAIL; // Only operator PRESENT does not require a value
					}
					// Find the subject
					if(MLD_getObjectWithIndex(object.subject, buffer, length, &object) > 0) {
						cmp->conditions[cmp->condition_count].object_type = object.type;
						cmp->condition_count++; // Move to next slot
					}
					else return FAIL; // Must have an object to compare to
				}
				else return FAIL; // Must have a subject
			}
			else if(MLObjects_isTemporalObject(object.type)) {
				if(cmp->temporal_count < MAX_TEMPORAL) {
					if(MLDTemporal_getIntervalWithIndex(position, buffer, length, &(cmp->temporal[cmp->temporal_count].interval)) > 0) {
						if(cmp->temporal[cmp->temporal_count].interval.subject > 0) {
							if(MLD_getObjectWithIndex(cmp->temporal[cmp->temporal_count].interval.subject, buffer, length, &object) > 0) {
								cmp->temporal[cmp->temporal_count].object_type = object.type;
								cmp->temporal_count++;
							}
							else return FAIL; // Subject must point to a valid object
						}
						else {
							cmp->temporal[cmp->temporal_count].object_type = dt_none;
							cmp->temporal_count++;
						}
					}
					else return FAIL; // The temporal interval is probably formatted incorrectly
				}
			}
			else if(MLObjects_isSpatialObject(object.type)) {
				if(cmp->spatial_count < MAX_SPATIAL) {
					if(MLDSpatial_getIntervalWithIndex(position, buffer, length, &(cmp->spatial[cmp->spatial_count].interval)) > 0) {
						if(cmp->spatial[cmp->spatial_count].interval.subject > 0) {
							if(MLD_getObjectWithIndex(cmp->spatial[cmp->spatial_count].interval.subject, buffer, length, &object) > 0) {
								cmp->spatial[cmp->spatial_count].object_type = object.type;
								cmp->spatial_count++;
							}
							else return FAIL; // Subject must point to a valid object
						}
						else {
							cmp->spatial[cmp->spatial_count].object_type = dt_none;
							cmp->spatial_count++;
						}
					}
					else return FAIL; // The temporal interval is probably formatted incorrectly
				}
			}
			position++;
		}
		if((cmp->condition_count > 0) || (cmp->spatial_count > 0) || (cmp->temporal_count > 0)) { // Comparisons make sense if we have at least one condition
			cmp->initialized = TRUE;
			return SUCCESS;
		}
		return FAIL;
	}

	bool MLCompare_satisfies(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) {
		if(cmp->initialized == TRUE) {
			if(MLCompare_satisfiesSimpleConditions(cmp, buffer, length)) {
				if(MLCompare_satisfiesTemporalConditions(cmp, buffer, length)) {
					if(MLCompare_satisfiesSpatialConditions(cmp, buffer, length)) return TRUE; // Went through all conditions and all were met
				}
			}
		}
		return FALSE; // Conditions not initialized
	}

	bool MLCompare_satisfiesSimpleConditions(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) {
		ml_object_t object;
		uint8_t ndex = 0;
		uint8_t i = 0;

		for(i=0;i<cmp->condition_count;i++) {
			ndex = MLD_findObjectWithParameters(cmp->conditions[i].object_type, NULL, NULL, buffer, length, &object);
			if(ndex > 0) { // Look for the specified object
				if(object.valueIsPresent == TRUE) {
					// TODO exp handling not done
					switch(cmp->conditions[i].oper) {
						case dt_present: // Value is not important
							break;
						case dt_less:
							if(!(object.value < cmp->conditions[i].object_value)) {
								MLCompare_printFailReason(object.type, cmp->conditions[i].oper, cmp->conditions[i].object_value, object.value);
								return FALSE;
							}
							break;
						case dt_greater:
							if(!(object.value > cmp->conditions[i].object_value)) {
								MLCompare_printFailReason(object.type, cmp->conditions[i].oper, cmp->conditions[i].object_value, object.value);
								return FALSE;
							}
							break;
						case dt_equal:
							if(!(object.value == cmp->conditions[i].object_value)) {
								MLCompare_printFailReason(object.type, cmp->conditions[i].oper, cmp->conditions[i].object_value, object.value);
								return FALSE;
							}
							break;
						case dt_lessEqual:
							if(!(object.value <= cmp->conditions[i].object_value)) {
								MLCompare_printFailReason(object.type, cmp->conditions[i].oper, cmp->conditions[i].object_value, object.value);
								return FALSE;
							}
							break;
						case dt_greaterEqual:
							if(!(object.value >= cmp->conditions[i].object_value)) {
								MLCompare_printFailReason(object.type, cmp->conditions[i].oper, cmp->conditions[i].object_value, object.value);
								return FALSE;
							}
							break;
						default:
							return FALSE;
					}
				}
				else if(cmp->conditions[i].oper != dt_present) { // Value is not present, but operator requires value
					MLCompare_printFailReasonNotPresent(cmp->conditions[i].object_type, TRUE);
					return FALSE;
				}
			}
			else { // Object is not present
				MLCompare_printFailReasonNotPresent(cmp->conditions[i].object_type, FALSE);
				return FALSE;
			}
		}
		return TRUE; // Went through all conditions and all were met
	}

	bool MLCompare_satisfiesTemporalConditions(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) {
		uint8_t i = 0;
		uint8_t j = 0;
		temporal_interval_t interval;
		ml_object_t object;
		uint8_t ndex = 0;
		bool ok = FALSE;
		for(i=0;i<cmp->temporal_count;i++) {
			if(cmp->temporal[i].object_type == dt_none) {
				ok = FALSE;
				j = 1;
				while(MLD_getObjectWithIndex(j, buffer, length, &object) > 0) {
					if(MLObjects_isTemporalObject(object.type)) {
						if(MLDTemporal_getIntervalWithIndex(j, buffer, length, &interval) > 0) {
							if(MLDTemporal_overlap(&interval, &(cmp->temporal[i].interval))) {
								ok = TRUE; // Condition met
								break; // while: general temporal constraint, one match is enough
							}
						}
					}
					j++;
				}
				if(!ok) return FALSE; // General constraint not met
			}
			else {
				// TODO does not handle cases of multiple objects and multiple intervals properly, checks only the first
				ndex = MLD_findObjectWithParameters(cmp->temporal[i].object_type, NULL, NULL, buffer, length, &object);
				if(ndex > 0) {
					if(MLDTemporal_findIntervalWithSubject(ndex, buffer, length, &interval) > 0) {
						if(!(MLDTemporal_overlap(&interval, &(cmp->temporal[i].interval)))) return FALSE; // Condition not met
					}
					else return FALSE; // The object does not have a spatial interval
				}
				else return FALSE; // No object
			}
		}
		return TRUE;
	}

	bool MLCompare_satisfiesSpatialConditions(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) {
		uint8_t i = 0;
		uint8_t j = 0;
		spatial_interval_t interval;
		ml_object_t object;
		uint8_t ndex = 0;
		bool ok = FALSE;
		for(i=0;i<cmp->spatial_count;i++) {
			if(cmp->spatial[i].object_type == dt_none) {
				ok = FALSE;
				j = 1;
				while(MLD_getObjectWithIndex(j, buffer, length, &object) > 0) {
					if(MLObjects_isSpatialObject(object.type)) {
						if(MLDSpatial_getIntervalWithIndex(j, buffer, length, &interval) > 0) {
							if(MLDSpatial_overlap(&interval, &(cmp->spatial[i].interval))) {
								ok = TRUE; // Condition met
								break; // while: general temporal constraint, one match is enough
							}
						}
					}
					j++;
				}
				if(!ok) return FALSE; // General constraint not met
			}
			else {
				// TODO does not handle cases of multiple objects and multiple intervals properly, checks only the first
				ndex = MLD_findObjectWithParameters(cmp->spatial[i].object_type, NULL, NULL, buffer, length, &object);
				if(ndex > 0) {
					if(MLDSpatial_findIntervalWithSubject(ndex, buffer, length, &interval) > 0) {
						if(!(MLDSpatial_overlap(&interval, &(cmp->spatial[i].interval)))) {
							MLCompare_printFailReason(cmp->spatial[i].object_type, cmp->spatial[i].interval.type, 0, 0);
							return FALSE; // Condition not met
						}
					}
					else {
						MLCompare_printFailReasonNotPresent(cmp->spatial[i].interval.type, FALSE);
						return FALSE; // The object does not have a spatial interval
					}
				}
				else {
					MLCompare_printFailReasonNotPresent(cmp->spatial[i].object_type, FALSE);
					return FALSE; // No object
				}
			}
		}
		return TRUE; // All conditions met
	}

	/*
	 * Debugging functions
	 * */

	void MLCompare_printConditions(ml_compare_t* cmp) {
		printf("cnd\toper\ttype\tvalue\n");
		for(int i=0;i<cmp->condition_count;i++) {
			char oper[3] = {0, 0, 0};
			MLCompare_operatorToString(cmp->conditions[i].oper, oper, 3);
			printf("%i\t%s\t%i\t%i\n", i, oper, cmp->conditions[i].object_type, cmp->conditions[i].object_value);
		}
		for(int i=0;i<cmp->spatial_count;i++) {
			char oper[3] = {0, 0, 0};
			MLCompare_operatorToString(cmp->spatial[i].interval.type, oper, 3);
			printf("%i\t%s\t%i\t%i\n", i, oper, cmp->spatial[i].object_type, 0);
		}
		for(int i=0;i<cmp->temporal_count;i++) {
			char oper[3] = {0, 0, 0};
			MLCompare_operatorToString(cmp->temporal[i].interval.type, oper, 3);
			printf("%i\t%s\t%i\t%i\t%i\n", i, oper, cmp->temporal[i].object_type, cmp->temporal[i].interval.start, cmp->temporal[i].interval.intTime);
		}
		printf("\n");
	}

	void MLCompare_printFailReasonNotPresent(int32_t type, bool value) {
		printf("%d %s not present\n", type, value ? "value": "");
	}

	void MLCompare_printFailReason(int32_t type, uint8_t oper, int32_t cvalue, int32_t ovalue) {
		char soper[4] = {0, 0, 0, 0};
		MLCompare_operatorToString(oper, soper, 3);
		printf("%d value: %d %s %d\n", type, ovalue, soper, cvalue);
	}

	error_t MLCompare_operatorToString(uint8_t value, char *buf, uint8_t length) {
		if(length >= 3) {
			switch(value) {
				case dt_present:
					buf[0] = '?';
					buf[1] = 0;
					break;
				case dt_less:
					buf[0] = '<';
					buf[1] = 0;
					break;
				case dt_greater:
					buf[0] = '>';
					buf[1] = 0;
					break;
				case dt_equal:
					buf[0] = '=';
					buf[1] = '=';
					buf[2] = 0;
					break;
				case dt_lessEqual:
					buf[0] = '<';
					buf[1] = '=';
					buf[2] = 0;
					break;
				case dt_greaterEqual:
					buf[0] = '>';
					buf[1] = '=';
					buf[2] = 0;
					break;
				case dt_spiC:
					buf[0] = 'C';
					buf[1] = 0;
					break;
				case dt_spiA:
					buf[0] = 'A';
					buf[1] = 0;
					break;
				case dt_spiV:
					buf[0] = 'V';
					buf[1] = 0;
					break;
				case dt_timeAbs:
					buf[0] = 'T';
					buf[1] = 'A';
					buf[2] = 0;
					break;
				default:
					return FAIL;
			}
			return SUCCESS;
		}
		return FAIL;
	}
