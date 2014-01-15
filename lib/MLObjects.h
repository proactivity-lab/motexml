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
 * @brief Object type function headers.
 *
 * @author Raido Pahtma
 */
#ifndef MLOBJECTS_H_
#define MLOBJECTS_H_
#include "ML.h"

	/**
	 * Check if the object type is one of <,>,<=,>=,==
	 *
	 * @param type Object type.
	 * @return TRUE or FALSE
	 */
	bool MLObjects_isSimpleConditionObject(int32_t type) ATTR_TINYOS_AT_C;

	/**
	 * Check if the object type is one of known temporal types.
	 *
	 * @param type Object type.
	 * @return TRUE or FALSE
	 */
	bool MLObjects_isSpatialObject(int32_t type) ATTR_TINYOS_AT_C;

	/**
	 * Check if the object type is one of known spatial types.
	 *
	 * @param type Object type.
	 * @return TRUE or FALSE
	 */
	bool MLObjects_isTemporalObject(int32_t type) ATTR_TINYOS_AT_C;

#endif /* MLOBJECTS_H_ */
