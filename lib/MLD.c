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
 * @brief Decode functions.
 *
 * @author Raido Pahtma
 */
#include "MLD.h"

#include <stdio.h> // Needed for NULL
#include <stdbool.h>

	uint8_t pMLD_readUnsignedByte(const uint8_t* bstart) {
		return *bstart;
	}

	int32_t pMLD_readInteger(uint8_t num, const uint8_t* bstart) {
		switch(num) {
			case 1:
				return *((int8_t*)bstart);
			case 2:
				return *((int16_t*)bstart);
			case 4:
				return *((int32_t*)bstart);
			default:
				return 0;
		}
	}

	uint32_t pMLD_readUnsignedInteger(uint8_t num, const uint8_t* bstart) {
		switch(num) {
			case 1:
				return *((uint8_t*)bstart);
			case 2:
				return *((uint16_t*)bstart);
			case 4:
				return *((uint32_t*)bstart);
			default:
				return 0;
		}
	}

	uint16_t pMLD_getObjectAt(uint16_t offset, const uint8_t buffer[], uint16_t length, ml_object_t* object) {
		uint8_t subjectSize = (buffer[offset] & ML_SUBJ_LOC) >> ML_SUBJ_SHF;
		uint8_t typeSize    = (buffer[offset] & ML_TYPE_LOC) >> ML_TYPE_SHF;
		uint8_t valueSize   = (buffer[offset] & ML_VALU_LOC) >> ML_VALU_SHF;
		uint16_t nextOffset = offset + 1 + subjectSize + typeSize + valueSize;

		if(subjectSize == 1) object->subject = pMLD_readUnsignedByte(buffer + offset + 1);
		else object->subject = 0;

		object->type = pMLD_readUnsignedInteger(typeSize, buffer + offset + 1 + subjectSize);
		if(valueSize > 0) {
			object->value = pMLD_readInteger(valueSize, buffer + offset + 1 + subjectSize + typeSize);
			object->valueIsPresent = true;
		}
		else object->valueIsPresent = false;

		if((buffer[offset] & ML_BBUF_LOC)) {
			object->bufferLength = pMLD_readUnsignedByte(buffer + nextOffset);
			// FIXME: here we drop const-ness because ml_object_t is shared
			// between decoder and encoder and buffer cannot be const in latter
			// use case. Might want to use separate structs for those cases?
			object->buffer = buffer + nextOffset + 1;
			nextOffset = nextOffset + 1 + object->bufferLength;
		}
		else {
			object->bufferLength = 0;
			object->buffer = NULL;
		}

		if(typeSize > 0) return nextOffset;

		return length + 1; // Over the end of the buffer
	}

	uint8_t pMLD_getTypeAt(uint16_t offset, const uint8_t buffer[], uint16_t length, uint32_t* type) {
		uint8_t typeSize = (buffer[offset] & ML_TYPE_LOC) >> ML_TYPE_SHF;
		*type = pMLD_readUnsignedInteger(typeSize, buffer + offset + 1);
		return typeSize;
	}

	uint16_t pMLD_getNextOffset(uint16_t offset, const uint8_t buffer[], uint16_t length) {
		uint8_t subjectSize = (buffer[offset] & ML_SUBJ_LOC) >> ML_SUBJ_SHF;
		uint8_t typeSize    = (buffer[offset] & ML_TYPE_LOC) >> ML_TYPE_SHF;
		uint8_t valueSize   = (buffer[offset] & ML_VALU_LOC) >> ML_VALU_SHF;
		uint16_t nextOffset = offset + 1 + subjectSize + typeSize + valueSize;

		if((buffer[offset] & ML_BBUF_LOC)) { // A byte buffer
			nextOffset = nextOffset + 1 + pMLD_readUnsignedByte(buffer + nextOffset); // Adds 1 length byte and data bytes
		}

		if(typeSize > 0) return nextOffset;

		return length + 1; // Over the end of the buffer
	}

	uint16_t pMLD_getOffsetOf(const uint8_t buffer[], uint16_t length, uint8_t number) {
		uint8_t i = 0;
		uint16_t offset = 0;

		// Shuffle through the objects
		for(i=1;i<number;i++) {
			offset = pMLD_getNextOffset(offset, buffer, length);
			if(offset > length) return length + 1; // Went over the end of the buffer
		}
		return offset;
	}

	// Public

	uint8_t MLD_getObjectWithIndex(uint8_t ndex, uint8_t const buffer[], uint16_t length, ml_object_t *object) {
		uint8_t i = 0;
		uint16_t offset = 0;

		// Shuffle through the objects
		for(i=1;i<ndex;i++) {
			offset = pMLD_getNextOffset(offset, buffer, length);
			if(offset > length) return 0; // Went over the end of the buffer
		}
		// Get the object with the desired index
		offset = pMLD_getObjectAt(offset, buffer, length, object);
		if(offset <= length) {
			object->index = ndex;
			return ndex;
		}
		return 0;
	}

	uint8_t MLD_findObjectWithParameters(uint32_t type, const uint8_t* subject, const int32_t* value, const uint8_t buffer[], uint16_t length, ml_object_t* object) {
		uint16_t offset = 0;
		uint8_t position = 0;

		while(offset < length) {
			position++;
			offset = pMLD_getObjectAt(offset, buffer, length, object);
			if(offset <= length) { // Extracted a valid object
				if(object->type == type) {
					/* Get subject */
					if((subject == NULL) || (object->subject == *subject)) { // If subject interests us, check that it's there and compare
						/* Get value */
						if((value == NULL) || (object->valueIsPresent && (object->value == *value))) { // If value interests us, check that it's there and compare
							object->index = position;
							return position;
						}
					}
				}
			}
			else break; // no valid object found
		}
		return 0;
	}

	uint8_t MLD_findOSV(uint32_t type, uint8_t subject, int32_t value, const uint8_t buffer[], uint16_t length, ml_object_t* object) {
		return MLD_findObjectWithParameters(type, &subject, &value, buffer, length, object);
	}

	uint8_t MLD_findOV(uint32_t type, int32_t value, const uint8_t buffer[], uint16_t length, ml_object_t* object) {
		return MLD_findObjectWithParameters(type, NULL, &value, buffer, length, object);
	}

	uint8_t MLD_findOS(uint32_t type, uint8_t subject, const uint8_t buffer[], uint16_t length, ml_object_t* object) {
		return MLD_findObjectWithParameters(type, &subject, NULL, buffer, length, object);
	}

	uint8_t MLD_findO(uint32_t type, const uint8_t buffer[], uint16_t length, ml_object_t* object) {
		return MLD_findObjectWithParameters(type, NULL, NULL, buffer, length, object);
	}

	uint8_t MLD_getObjectCount(const uint8_t buffer[], uint16_t length) {
		uint16_t offset = 0;
		uint8_t position = 0;

		while(offset < length) {
			offset = pMLD_getNextOffset(offset, buffer, length);
			if(offset != 0) position++;
			else return position;
		}
		return position;
	}
