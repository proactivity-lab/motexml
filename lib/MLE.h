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
 * @brief Encode function headers.
 *
 * @author Raido Pahtma
 */
#ifndef MLE_H_
#define MLE_H_
#include "ML.h"

#include <stdio.h>

	/**
	 * ml_encoder_t
	 */
	typedef struct ml_encoder_t {
		uint8_t* buf; /**< Buffer pointer */
		uint16_t usedSpace; /**< Currently used space */
		uint16_t totalSpace; /**< Size of the buffer */
		uint8_t currentPosition; /**< Current count of objects */
		uint8_t errors; /**< Number of errors encountered during encode calls. */
	} ml_encoder_t;

	/**
	 * Initializes the encoder structure with the provided buffer information.
	 *
	 * @param enc
	 * @param buffer
	 * @param length
	 * @return
	 */
	LIBEXPORT ml_error_t MLE_initialize(ml_encoder_t* enc, uint8_t buffer[], uint16_t length);

	/**
	 * Finalizes the buffer.
	 *
	 * @return Length of the data in the buffer.
	 */
	LIBEXPORT uint16_t MLE_finalize(ml_encoder_t* enc);

	/**
	 * Reinitialize an encoder with the buffer for additional appends and editing.
	 *
	 * @param enc An encoder to be used.
	 * @param buffer The buffer.
	 * @param length Size of the current contents of the buffer.
	 * @param totalLength Total length of the buffer, >= length.
	 * @return SUCCESS, if the contents of the buffer made sense.
	 */
	LIBEXPORT ml_error_t MLE_reinitialize(ml_encoder_t* enc, uint8_t buffer[], uint16_t length, uint16_t totalLength);

	/**
	 * Append object with the specified parameters.
	 *
	 * @param enc The encoder.
	 * @param type Object type.
	 * @param subject Subject, can be 0.
	 * @param value - Value, can be NULL.
	 * @return
	 */
	LIBEXPORT uint8_t MLE_appendObjectWithValues(ml_encoder_t* enc, uint32_t type, uint8_t subject, int32_t* value);

	/**
	 * Append object.
	 *
	 * @param enc The encoder.
	 * @param object - The object.
	 * @return
	 */
	LIBEXPORT uint8_t MLE_appendObject(ml_encoder_t* enc, ml_object_t* object);

	/**
	 * Append object with the specified parameters.
	 *
	 * @param enc The encoder.
	 * @param type
	 * @param subject
	 * @param value
	 * @return
	 */
	LIBEXPORT uint8_t MLE_appendOSV(ml_encoder_t* enc, uint32_t type, uint8_t subject, int32_t value);

	/**
	 * Append top-level object with value.
	 *
	 * @param enc The encoder.
	 * @param type
	 * @param value
	 * @return
	 */
	LIBEXPORT uint8_t MLE_appendOV(ml_encoder_t* enc, uint32_t type, int32_t value);

	/**
	 * Append object with subject, but no value.
	 *
	 * @param enc The encoder.
	 * @param type
	 * @param subject
	 * @return
	 */
	LIBEXPORT uint8_t MLE_appendOS(ml_encoder_t* enc, uint32_t type, uint8_t subject);

	/**
	 * Append a top-level object with no value.
	 * @param enc The encoder.
	 * @param type
	 * @return
	 */
	LIBEXPORT uint8_t MLE_appendO(ml_encoder_t* enc, uint32_t type);

	/**
	 * Append an object with a buffer and no value.
	 * @param enc The encoder.
	 * @param type Object type.
	 * @param subject Subject, can be 0.
	 * @param buffer - Raw data buffer.
	 * @param length - Data buffer length.
	 * @return
	 */
	LIBEXPORT uint8_t MLE_appendOSB(ml_encoder_t* enc, uint32_t type, uint8_t subject, uint8_t buffer[], uint8_t length);

	/**
	 * Append an object with a buffer and value.
	 * @param enc The encoder.
	 * @param type Object type.
	 * @param subject Subject, can be 0.
	 * @param value - Value, can be NULL.
	 * @param buffer - Raw data buffer.
	 * @param length - Data buffer length.
	 * @return
	 */
	LIBEXPORT uint8_t MLE_appendOSVB(ml_encoder_t* enc, uint32_t type, uint8_t subject, int32_t value, uint8_t buffer[], uint8_t length);

	LIBEXPORT uint16_t MLE_spaceLeft(ml_encoder_t* enc);

	LIBEXPORT uint16_t MLE_spaceUsed(ml_encoder_t* enc);

	LIBEXPORT uint8_t MLE_errors(ml_encoder_t* enc);

	/**
	 * Deletes the object at the specified location, and all sub-objects.
	 *
	 * @return Index of the deleted object or 0 if failed to delete. The buffer is no longer valid,
	 * if 0 was returned(probably was invalid before and that is why the delete failed).
	 */
	LIBEXPORT uint8_t MLE_deleteObject(ml_encoder_t* enc, uint8_t number);

	/* Protected */

    uint8_t pMLE_append(ml_encoder_t* enc, uint32_t type, uint8_t subject, int32_t* value, uint8_t buffer[], uint16_t length);
	uint8_t pMLE_sizeOf(int32_t value);
	uint8_t pMLE_sizeOfUnsigned(uint32_t value);
	void pMLE_appendUnsignedByte(ml_encoder_t* enc, uint8_t value);
	void pMLE_appendInteger(ml_encoder_t* enc, uint8_t size, int32_t value);
	void pMLE_appendUnsignedInteger(ml_encoder_t* enc, uint8_t size, uint32_t value);
	uint8_t pMLE_deleteObjectAt(ml_encoder_t* enc, uint16_t offset, uint8_t number);

	/**
	 * Information for memory allocation in Python.
	 */
	LIBEXPORT uint8_t MLE_encoderSize(void);

#endif /* MLE_H_ */
