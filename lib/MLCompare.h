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
 * @brief Comparison function headers.
 *
 * @details MLCompare
 *
 * Tool for comparing ML messages.
 *
 * MLCompare assumes that a buffer contains one of each object that is to be referenced in a query.
 * Common comparison operators look at the value of the object. Spatial and temporal interval operators
 * look at the intervals for the object type. What this means is that when the comparator is initialized,
 * then comparison operations are created from all <,>,<=,>=,== operators, containing a type to be referenced
 * and a value(where applicable). Special operations are created for supported temporal or spatial intervals,
 * parsing all the necessary fields from the buffer. An interval is essentially handled as a condition.
 * If an interval constraint does not have an accompanying object, then any overlapping interval will satisfy the
 * condition.
 * All other objects are ignored unless they are referred to by the operators.
 *
 * This version limits buffer length to 255 bytes. The maximum number of simple, spatial and temporal conditions
 * is specified with constructor arguments. This is done to allow easy porting to TinyOS, memory is allocated
 * in constructor or at compile time in TinyOS. A more dynamic implementation would not need to be limited in this way.
 *
 * @author Raido Pahtma
 */
#ifndef MLCOMPARE_H_
#define MLCOMPARE_H_
#include "ML.h"

/**
 * Spatial condition object struct.
 */
	typedef struct spat_condition_t {
		int32_t object_type;
		spatial_interval_t interval;
	} spat_condition_t;

/**
 * Temporal condition object struct
 */
	typedef struct temp_condition_t {
		int32_t object_type;
		temporal_interval_t interval;
	} temp_condition_t;

/**
 * Condition object struct.
 */
	typedef struct ml_condition_t {
		int8_t oper;
		int32_t object_type;
		int32_t object_value;
	} ml_condition_t;

#define MAX_CONDITIONS 3
#define MAX_SPATIAL 1
#define MAX_TEMPORAL 1

	typedef struct ml_compare_t {
		bool initialized;

		ml_condition_t conditions[MAX_CONDITIONS];
		spat_condition_t spatial[MAX_SPATIAL];
		temp_condition_t temporal[MAX_TEMPORAL];
		uint8_t condition_count;
		uint8_t spatial_count;
		uint8_t temporal_count;
	} ml_compare_t;

	/**
	 * Initialize the object for conditions described in the buffer.
	 * No need to uninitialize, just initialize again with a new buffer.
	 *
	 * @param buffer Conditions in ML format.
	 * @param length Buffer length.
	 * @return SUCCESS if no errors encountered.
	 */
	error_t MLCompare_initialize(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) ATTR_TINYOS_AT_C;

	/**
	 *	Check if the entry described in the buffer satisfies the initialized conditions.
	 *
	 * @param buffer Entry in ML format.
	 * @param length Buffer length.
	 * @return FALSE if does not satisfy or not initialized.
	 */
	bool MLCompare_satisfies(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) ATTR_TINYOS_AT_C;

	bool MLCompare_satisfiesSimpleConditions(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) ATTR_TINYOS_AT_C;
	bool MLCompare_satisfiesTemporalConditions(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) ATTR_TINYOS_AT_C;
	bool MLCompare_satisfiesSpatialConditions(ml_compare_t* cmp, uint8_t buffer[], uint8_t length) ATTR_TINYOS_AT_C;

	/* Debug functions */

	void MLCompare_printConditions() ATTR_TINYOS_AT_C;

	/**
	 * Debug function
	 *
	 * @param type
	 * @param value
	 */
	void MLCompare_printFailReasonNotPresent(int32_t type, bool value) ATTR_TINYOS_AT_C;

	/**
	 * Debug function
	 *
	 * @param type
	 * @param oper
	 * @param cvalue
	 * @param ovalue
	 */
	void MLCompare_printFailReason(int32_t type, uint8_t oper, int32_t cvalue, int32_t ovalue) ATTR_TINYOS_AT_C;

	/**
	 * Debug function
	 *
	 * @param value
	 * @param buf
	 * @param length
	 * @return
	 */
	error_t MLCompare_operatorToString(uint8_t value, char *buf, uint8_t length) ATTR_TINYOS_AT_C;

#endif /* MLCOMPARE_H_ */
