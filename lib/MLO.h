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
 * @brief Object function headers.
 *
 * @author Raido Pahtma
 */
#ifndef MLO_H_
#define MLO_H_
#include "ML.h"

	LIBEXPORT uint8_t MLO_getIndex(ml_object_t* object);

	LIBEXPORT uint32_t MLO_getType(ml_object_t* object);

	LIBEXPORT int32_t MLO_getValue(ml_object_t* object);

	LIBEXPORT bool MLO_getValueIsPresent(ml_object_t* object);

	LIBEXPORT uint8_t MLO_getSubject(ml_object_t* object);

	LIBEXPORT uint8_t* MLO_getBuffer(ml_object_t* object);

	LIBEXPORT uint8_t MLO_getBufferLength(ml_object_t* object);

	LIBEXPORT void MLO_setIndex(ml_object_t* object, uint8_t ndex);

	LIBEXPORT void MLO_setType(ml_object_t* object, uint32_t type);

	LIBEXPORT void MLO_setValue(ml_object_t* object, int32_t value);

	LIBEXPORT void MLO_setValueIsPresent(ml_object_t* object, bool present);

	LIBEXPORT void MLO_setSubject(ml_object_t* object, uint8_t subject);

	LIBEXPORT void MLO_setBuffer(ml_object_t* object, uint8_t buffer[], uint8_t length);

	LIBEXPORT uint8_t MLO_objectSize();

#endif /* MLO_H_ */
