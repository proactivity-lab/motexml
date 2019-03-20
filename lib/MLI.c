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
 * @brief Iteration functions.
 *
 * @author Raido Pahtma
 */
#include "MLI.h"
#include "MLD.h"

#include <stdio.h> // Needed for NULL

	LIBEXPORT ml_error_t MLI_initialize(ml_iterator_t* iter, uint8_t buffer[], uint16_t length) {
		iter->buf = buffer;
		iter->length = length;
		return MLI_reset(iter);
	}

	LIBEXPORT ml_error_t MLI_reset(ml_iterator_t* iter) {
		if(iter->buf != NULL) {
			iter->offset = 0;
			iter->nextOffset = 0;
			iter->index = 0;
			return ML_SUCCESS;
		}
		return ML_FAIL;
	}

	LIBEXPORT uint8_t MLI_next(ml_iterator_t* iter, ml_object_t* object) {
		if(iter->nextOffset < iter->length) {
			iter->offset = iter->nextOffset;
			iter->nextOffset = pMLD_getObjectAt(iter->nextOffset, iter->buf, iter->length, object);
			if(iter->nextOffset <= iter->length) { // The object is valid
				iter->index++;
				object->index = iter->index;
				return iter->index;
			}
		}
		return 0;
	}

	LIBEXPORT uint8_t MLI_nextWithSubject(ml_iterator_t* iter, uint8_t subject, ml_object_t* object) {
		uint8_t ndex = 0;
		while((ndex = MLI_next(iter, object)) > 0) {
			if(object->subject == subject) return ndex;
		}
		return 0;
	}

	LIBEXPORT uint8_t MLI_nextWithType(ml_iterator_t* iter, uint32_t type, ml_object_t* object) {
		uint8_t ndex = 0;
		while((ndex = MLI_next(iter, object)) > 0) {
			if(object->type == type) return ndex;
		}
		return 0;
	}

	LIBEXPORT uint8_t MLI_icopy(uint8_t sindex, uint8_t* sb, uint16_t slength, uint8_t dsubject, ml_encoder_t* enc) {
		ml_encoder_t backup = *enc;
		ml_iterator_t iter;
		ml_object_t object;
		if(MLI_initialize(&iter, sb, slength) == ML_SUCCESS) {
			uint8_t sstack[MLI_ICOPY_MAX_STACK];
			uint8_t dstack[MLI_ICOPY_MAX_STACK];
			uint16_t iOffset[MLI_ICOPY_MAX_STACK];
			uint16_t iNextOffset[MLI_ICOPY_MAX_STACK];
			uint8_t iIndex[MLI_ICOPY_MAX_STACK];
			uint8_t stptr = 0;
			uint8_t ssub = sindex;
			uint8_t dsub = dsubject;
			uint8_t ret = 0; // Index of the root of the copied tree
			if(sindex > 0) {
				if(MLD_getObjectWithIndex(sindex, sb, slength, &object) > 0) {
					object.subject = dsubject;
					dsub = MLE_appendObject(enc, &object);
					ret = dsub;
					if(dsub == 0) {
						enc->errors++;
						return 0; // return EINVAL;
					}
				}
				else {
					enc->errors++;
					return 0; // return ML_FAIL;
				}
			}
			while(stptr < MLI_ICOPY_MAX_STACK) {
				uint8_t ndex = MLI_nextWithSubject(&iter, ssub, &object);
				if(ndex > 0) {
					//printf("%02u: s=%u d=%u\n", ndex, ssub, dsub);
					iOffset[stptr] = iter.offset;
					iNextOffset[stptr] = iter.nextOffset;
					iIndex[stptr] = iter.index;
					sstack[stptr] = ssub;
					dstack[stptr] = dsub;
					ssub = ndex;
					object.subject = dsub;
					dsub = MLE_appendObject(enc, &object);
					if(dsub == 0) {
						*enc = backup;
						enc->errors++;
						return 0; // return EINVAL;
					}
					if(ret == 0)
					{
						ret = dsub;
					}
					stptr++;
					MLI_reset(&iter);
				}
				else {
					if(stptr == 0) {
						return ret; // return ML_SUCCESS;
					}
					stptr--;
					iter.offset = iOffset[stptr];
					iter.nextOffset = iNextOffset[stptr];
					iter.index = iIndex[stptr];
					ssub = sstack[stptr];
					dsub = dstack[stptr];
				}
			}
			*enc = backup; // Restore encoder to previous state.
			// return ENOMEM; // Hit the MLI_ICOPY_MAX_STACK limit
		}
		else {
			// return EINVAL; // Failed to append stuff to the new buffer
		}
		enc->errors++;
		return 0; // Return ML_FAIL;
	}

	uint16_t MLI_getCurrentOffset(ml_iterator_t* iter) {
		return iter->offset;
	}

	uint16_t MLI_getNextOffset(ml_iterator_t* iter) {
		return iter->nextOffset;
	}

	LIBEXPORT uint8_t MLI_iteratorSize() {
		return sizeof(ml_iterator_t);
	}

