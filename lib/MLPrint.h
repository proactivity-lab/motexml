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
 * @brief Print function headers.
 *
 * @author Raido Pahtma
 */
#ifndef MLPRINT_H_
#define MLPRINT_H_

#include "MLText.h"
#include "MLD.h"
#include "MLObjects.h"

enum {
	MAX_PRINT_DEPTH = 10
};

class MLPrint {
public:

	void printObject(ml_object_t* object);
	void printTemporalInterval(temporal_interval_t* interval);
	void printSpatialInterval(spatial_interval_t* interval);

	void printObjectAndIntervals(int32_t type, uint8_t buffer[], uint8_t length);
	void printObjectsRecursively(uint8_t subject, uint8_t currentDepth, uint8_t buffer[], uint8_t length);

	void printObjects(uint8_t buffer[], uint8_t length);
	void printObjectsSequentially(uint8_t buffer[], uint8_t length);

	void printBuffer(uint8_t buffer[], uint8_t length);

	MLPrint(MLText* text);
	virtual ~MLPrint();

private:
	MLText* mlt;
};

#endif /* MLPRINT_H_ */
