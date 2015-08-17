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
 * @brief Iteration function headers.
 *
 * @author Raido Pahtma
 */
#ifndef MLI_H_
#define MLI_H_
#include "ML.h"
#include "MLE.h"

#ifndef __tinyos__
#include <stdio.h>
#endif /* __tinyos__ */

#ifndef MLI_ICOPY_MAX_STACK
#define MLI_ICOPY_MAX_STACK 12
#endif /* MLI_ICOPY_MAX_STACK */

	typedef struct ml_iterator_t {
		uint8_t* buf;
		uint16_t length;
		uint16_t offset;
		uint16_t nextOffset;
		uint8_t index;
	} ml_iterator_t;

	/**
	 * Initializes the iterator to the first object in the buffer.
	 *
	 * @param iter Pointer to a ml_iterator_t.
	 * @param buffer The message buffer.
	 * @param length Length of the message buffer.
	 *
	 * @return Normally SUCCESS, only FAIL if buffer is NULL.
	 */
	LIBEXPORT error_t MLI_initialize(ml_iterator_t* iter, uint8_t buffer[], uint16_t length) ATTR_TINYOS_AT_C;

	/**
	 * Resets the iterator to the first object in the previously provided buffer.
	 *
	 * @return Succeeds if the iterator has been previously initialized.
	 */
	LIBEXPORT error_t MLI_reset(ml_iterator_t* iter) ATTR_TINYOS_AT_C;

	/**
	 * Retrieves the next object from the buffer.
	 *
	 * @return Index of the object or 0 if no more objects in the buffer.
	 */
	LIBEXPORT uint8_t MLI_next(ml_iterator_t* iter, ml_object_t* object) ATTR_TINYOS_AT_C;

	/**
	 * Retrieves the next object with the specified subject value.
	 *
	 * @return Index of the object or 0 if no more objects in the buffer.
	 */
	LIBEXPORT uint8_t MLI_nextWithSubject(ml_iterator_t* iter, uint8_t subject, ml_object_t* object) ATTR_TINYOS_AT_C;

	/**
	 * Retrieves the next object with the specified type value.
	 *
	 * @return Index of the object or 0 if no more objects in the buffer.
	 */
	LIBEXPORT uint8_t MLI_nextWithType(ml_iterator_t* iter, uint32_t type, ml_object_t* object) ATTR_TINYOS_AT_C;

	/**
	 * Copy a subtree from sb to encoder with subject dsubject.
	 *
	 * @param sindex Index of the top-level object to be copied. 0 to copy whole message.
	 * @param sb Source buffer.
	 * @param slen Length of the source buffer.
	 * @param dsubject Subject of the top-level object to be copied in the new buffer.
	 * @param enc Initialized encoder for the new tree.
	 * @return 0 if fails(contents of enc are unaltered, but errors is incremented) index of copied tree root otherwise.
	 */
	LIBEXPORT uint8_t MLI_icopy(uint8_t sindex, uint8_t* sbuffer, uint16_t slength, uint8_t dsubject, ml_encoder_t* enc) ATTR_TINYOS_AT_C;

	/* Protected */
	uint16_t MLI_getCurrentOffset(ml_iterator_t* iter) ATTR_TINYOS_AT_C;
	uint16_t MLI_getNextOffset(ml_iterator_t* iter) ATTR_TINYOS_AT_C;

	/**
	 * Information for memory allocation in Python.
	 */
	LIBEXPORT uint8_t MLI_iteratorSize() ATTR_TINYOS_AT_C;

#endif /* MLI_H_ */
