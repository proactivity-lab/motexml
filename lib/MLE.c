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
 * @brief Encode functions.
 *
 * @author Raido Pahtma
 */
#include "ML.h"
#include "MLE.h"
#include "MLD.h"
#include "MLI.h"

#include "LOG.h"

#include <stdio.h> // Needed for NULL
#include <string.h>

	LIBEXPORT uint8_t MLE_appendObjectWithValues(ml_encoder_t* enc, uint32_t type, uint8_t subject, int32_t* value) {
		return pMLE_append(enc, type, subject, value, NULL, 0);
	}

	LIBEXPORT uint8_t MLE_appendObject(ml_encoder_t* enc, ml_object_t* object) {
		if(object != NULL) { // Initialization is currently checked by the other appendObject
			return pMLE_append(enc, object->type, object->subject, object->valueIsPresent ? &(object->value) : NULL, object->buffer, object->bufferLength);
		}
		return 0;
	}

	LIBEXPORT uint8_t MLE_appendOSV(ml_encoder_t* enc, uint32_t type, uint8_t subject, int32_t value) {
		return MLE_appendObjectWithValues(enc, type, subject, &value);
	}

	LIBEXPORT uint8_t MLE_appendOV(ml_encoder_t* enc, uint32_t type, int32_t value) {
		return MLE_appendObjectWithValues(enc, type, 0, &value);
	}

	LIBEXPORT uint8_t MLE_appendOS(ml_encoder_t* enc, uint32_t type, uint8_t subject) {
		return MLE_appendObjectWithValues(enc, type, subject, NULL);
	}

	LIBEXPORT uint8_t MLE_appendO(ml_encoder_t* enc, uint32_t type) {
		return MLE_appendObjectWithValues(enc, type, 0, NULL);
	}

	LIBEXPORT uint8_t MLE_appendOSB(ml_encoder_t* enc, uint32_t type, uint8_t subject, uint8_t buffer[], uint8_t length) {
		return pMLE_append(enc, type, subject, NULL, buffer, length);
	}

	LIBEXPORT uint8_t MLE_appendOSVB(ml_encoder_t* enc, uint32_t type, uint8_t subject, int32_t value, uint8_t buffer[], uint8_t length) {
		return pMLE_append(enc, type, subject, &value, buffer, length);
	}

	// Info and setup functions

	LIBEXPORT uint16_t MLE_spaceLeft(ml_encoder_t* enc) {
		if(enc->buf != NULL) {
			return enc->totalSpace - enc->usedSpace;
		}
		return 0;
	}

	LIBEXPORT uint16_t MLE_spaceUsed(ml_encoder_t* enc) {
		if(enc->buf != NULL) {
			return enc->usedSpace;
		}
		return 0;
	}

	LIBEXPORT uint8_t MLE_errors(ml_encoder_t* enc) {
		return enc->errors;
	}

	LIBEXPORT ml_error_t MLE_initialize(ml_encoder_t* enc, uint8_t buffer[], uint16_t length) {
		if(buffer != NULL && length >= 2) {
			enc->buf = buffer;
			enc->totalSpace = length;
			enc->usedSpace = 0;
			enc->currentPosition = 0;
			enc->errors = 0;
			return ML_SUCCESS;
		}
		enc->buf = NULL;
		enc->totalSpace = 0;
		enc->usedSpace = 0;
		enc->currentPosition = 0;
		enc->errors = 1;
		return ML_FAIL;
	}

	LIBEXPORT uint16_t MLE_finalize(ml_encoder_t* enc) {
		if(enc->buf != NULL) {
			enc->buf = NULL;
			return enc->usedSpace;
		}
		return 0;
	}

	LIBEXPORT ml_error_t MLE_reinitialize(ml_encoder_t* enc, uint8_t buffer[], uint16_t length, uint16_t totalLength) {
		if(buffer != NULL) {
			if(length >= 2) {
				enc->buf = buffer;
				enc->totalSpace = totalLength;
				enc->usedSpace = length;
				enc->currentPosition = MLD_getObjectCount(buffer, length);
				enc->errors = 0;
				if((enc->currentPosition > 0) && (enc->usedSpace > 0)) return ML_SUCCESS;
			}
			else {
				return MLE_initialize(enc, buffer, totalLength);
			}
		}
		enc->buf = NULL;
		enc->totalSpace = 0;
		enc->usedSpace = 0;
		enc->currentPosition = 0;
		enc->errors = 1;
		return ML_FAIL;
	}

	LIBEXPORT uint8_t MLE_deleteObject(ml_encoder_t* enc, uint8_t number) {
		ml_iterator_t iter;
		ml_object_t object;
		uint8_t i, j;
		uint16_t delOffset = 0;
		uint8_t currentDel = number;
		if(enc->buf != NULL) {
			if(MLI_initialize(&iter, enc->buf, enc->usedSpace) == ML_SUCCESS) {
				while(1) { // Loop forever :)
					i = 0;
					while((j = MLI_nextWithSubject(&iter, currentDel, &object)) > 0) {
						delOffset = MLI_getCurrentOffset(&iter); // Remember the offset
						i = j; // Get the last object with subject set to currentDel
					}
					if(i > 0) { // Found an object that we need to delete before we delete the desired object
						currentDel = i; // Change subject
						MLI_initialize(&iter, enc->buf, enc->usedSpace);
					}
					else {
						if(currentDel == number) { // Did not find any (more) sub-objects
							delOffset = pMLD_getOffsetOf(enc->buf, enc->usedSpace, number);
							if(delOffset < enc->usedSpace) { // Make sure that the object actually exists
								return pMLE_deleteObjectAt(enc, delOffset, number);
							}
							return 0;
						}
						else { // A sub-object without any sub-objects - can be removed
							if(pMLE_deleteObjectAt(enc, delOffset, currentDel) != currentDel) {
								return 0;
							}
							MLI_initialize(&iter, enc->buf, enc->usedSpace);
							currentDel = number; // Start looking for sub-objects again
						}
					}
				}
			}
		}
		return 0;
	}

	// Private/Protected functions

	uint8_t pMLE_append(ml_encoder_t* enc, uint32_t type, uint8_t subject, int32_t* value, uint8_t buffer[], uint16_t length) {
		uint8_t typeSize = pMLE_sizeOfUnsigned(type);
		uint8_t subjectSize = (subject != 0) ? 1 : 0;
		uint8_t valueSize = (value != NULL) ? pMLE_sizeOf(*value) : 0;
		uint8_t bufPresent = ((buffer != NULL) && (length > 0)) ? 1 : 0;
		uint16_t size = 1 + typeSize + subjectSize + valueSize + bufPresent + length;
		if(size <= MLE_spaceLeft(enc)) {
			uint8_t control = (bufPresent << ML_BBUF_SHF) | (subjectSize << ML_SUBJ_SHF) | (typeSize << ML_TYPE_SHF) | (valueSize << ML_VALU_SHF); // RSTTTVVV
			pMLE_appendUnsignedByte(enc, control);
			if(subjectSize == 1) pMLE_appendUnsignedByte(enc, subject);
			pMLE_appendUnsignedInteger(enc, typeSize, type);
			if(valueSize > 0) pMLE_appendInteger(enc, valueSize, *value);
			if(bufPresent) {
				pMLE_appendUnsignedByte(enc, length);
				memcpy(&enc->buf[enc->usedSpace], buffer, length);
				enc->usedSpace += length;
			}
			enc->currentPosition++;
			return enc->currentPosition;
		}
		enc->errors++;
		return 0; // Unsuccessful
	}

	uint8_t pMLE_sizeOf(int32_t value) {
		if(value == (int8_t)value) return 1;
		else if(value == (int16_t)value) return 2;
		return 4;
	}

	uint8_t pMLE_sizeOfUnsigned(uint32_t value) {
		if(value == (uint8_t)value) return 1;
		else if(value == (uint16_t)value) return 2;
		return 4;
	}

	void pMLE_appendUnsignedByte(ml_encoder_t* enc, uint8_t value) {
		enc->buf[enc->usedSpace] = value;
		enc->usedSpace++;
	}

	void pMLE_appendInteger(ml_encoder_t* enc, uint8_t size, int32_t value) {
		switch(size) {
			case 1:
				*((int8_t*)(enc->buf + enc->usedSpace)) = (int8_t)value;
				enc->usedSpace += 1;
				break;
			case 2:
				*(enc->buf + enc->usedSpace) = (value & 0xFF00);
				*(enc->buf + enc->usedSpace + 1) = (value & 0x00FF); // TODO MIGHT BE WRONG
				enc->usedSpace += 2;
				break;
			case 4:
				*((int32_t*)(enc->buf + enc->usedSpace)) = value;
				enc->usedSpace += 4;
				break;
			default:
				break;
		}
	}

	void pMLE_appendUnsignedInteger(ml_encoder_t* enc, uint8_t size, uint32_t value) {
		switch(size) {
			case 1:
				*((uint8_t*)(enc->buf + enc->usedSpace)) = (uint8_t)value;
				enc->usedSpace += 1;
				break;
			case 2:
				*((uint16_t*)(enc->buf + enc->usedSpace)) = (uint16_t)value;
				enc->usedSpace += 2;
				break;
			case 4:
				*((uint32_t*)(enc->buf + enc->usedSpace)) = value;
				enc->usedSpace += 4;
				break;
			default:
				break;
		}
	}

	uint8_t pMLE_deleteObjectAt(ml_encoder_t* enc, uint16_t offset, uint8_t number) {
		ml_object_t object;
		uint16_t i;
		uint16_t lastOffset;
		uint16_t nextOffset = pMLD_getNextOffset(offset, enc->buf, enc->usedSpace); // determine size of object
		uint16_t size = nextOffset - offset;
		if(nextOffset <= enc->usedSpace) {
			for(i=offset;i<enc->usedSpace - size;i++) { // move the rest of the buffer forward
				enc->buf[i] = enc->buf[i + size];
			}
			enc->usedSpace = enc->usedSpace - size;
			enc->currentPosition--;
			if(offset < enc->usedSpace) { // There are some objects after this one
				nextOffset = offset;
				while(nextOffset < enc->usedSpace) { // fix the subjects
					lastOffset = nextOffset;
					nextOffset = pMLD_getObjectAt(lastOffset, enc->buf, enc->usedSpace, &object);
					if(object.subject > number) { // Pointing to an object that has had its index changed
						enc->buf[lastOffset + 1]--; // Decrease subject
					}
				}
			}
			return number;
		}
		return 0;
	}

	LIBEXPORT uint8_t MLE_encoderSize() {
		return sizeof(ml_encoder_t);
	}
