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
 * @brief Language objects.
 *
 * @author Raido Pahtma
 */
#ifndef ML_H_
#define ML_H_
#include "prolab_types.h"
#include "dt_types.h"

/**
 * Decoded ML object struct.
 */
	typedef struct ml_object_t {
		uint8_t index; // The index of the decoded object. Has no effect in encode.
		uint32_t type; // The type of the object, dt_*.
		uint8_t subject; // Reference to an object above it.

		bool valueIsPresent; // The object might not have a value attached to it.
		int32_t value;

		uint8_t bufferLength; // The object may have a buffer associated with it.
		uint8_t* buffer; // The buffer pointing to memory inside the packet buffer on decode and to an external buffer on encode.
	} ml_object_t;

/**
 * Temporal intervals
 */
	typedef struct temporal_interval_t {
		uint32_t type;
		uint8_t subject;
		int32_t intTime;
		union {
			int32_t elapsed;
			uint32_t start;
		};
	} temporal_interval_t;

/**
 * Spatial intervals
 */
	typedef struct spiA_t {
		int32_t x[4];
		int32_t y[4];
	} spiA_t;

	typedef struct spiC_t {
		int32_t x;
		int32_t y;
		int32_t z;
		int32_t r;
	} spiC_t;

	typedef struct spiV_t {
		int32_t x[2];
		int32_t y[2];
	} spiV_t;

	typedef struct spiD_t {
		int32_t id;
	} spiD_t;

	typedef struct spatial_interval_t {
		uint32_t type; // spiC, spiV, spiA ...
		uint8_t subject;
		union {
			spiC_t c;
			spiV_t v;
			spiD_t d;
		};
	} spatial_interval_t;

	/*
	 * BSTTTVVV S8 T8 T8 T8 T8 V8 V8 V8 V8 BL8 B B B
	 */

	#define ML_BBUF_SHF 7
	#define ML_SUBJ_SHF 6
	#define ML_TYPE_SHF 3
	#define ML_VALU_SHF 0
	#define ML_BBUF_LOC 0x80 //0b10000000"
	#define ML_TYPE_LOC 0x38 //0b00111000"
	#define ML_SUBJ_LOC 0x40 //0b01000000"
	#define ML_VALU_LOC 0x07 //0b00000111"

#endif /* ML_H_ */
