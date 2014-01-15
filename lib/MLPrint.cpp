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
 * @brief Print functions.
 *
 * @author Raido Pahtma
 */
#include "ML.h"
#include "MLPrint.h"
#include "MLDSpatial.h"
#include "MLDTemporal.h"

void MLPrint::printObject(ml_object_t* object) {
	printf("T: %s(0x%x, %d) ", mlt->toString(object->type), object->type, object->type);
	if(object->valueIsPresent) printf("V: %d ", object->value);
	if(object->subject > 0) printf("S: %d ", object->subject);
	if(object->bufferLength > 0) {
		if(object->buffer != NULL) {
			printf("B: ");
			for(int i=0;i<object->bufferLength;i++) {
				printf("%02x", object->buffer[i]);
			}
		}
		else {
			printf("ERROR len=%u, buffer=NULL", object->bufferLength);
		}
	}
	else if(object->buffer != NULL)
	{
		printf("ERROR len=0, buffer=%p", (void*)object->buffer);
	}
	printf("\n");
}

void MLPrint::printTemporalInterval(temporal_interval_t* interval) {
	if(interval != NULL) {
		switch(interval->type) {
			case dt_timeAbs:
				printf("T: timeAbs(0x%x, %d) ", interval->type, interval->type);
				printf("Start at %u, duration %d seconds. ", interval->start, interval->intTime);
				break;
			case dt_timeR:
				printf("T: timeR(0x%x, %d) ", interval->type, interval->type);
				printf("Start %d seconds ago, duration %d seconds. ", interval->elapsed, interval->intTime);
				break;
			default:
				printf("T: (0x%x, %d) ", interval->type, interval->type);
				printf("Unknown temporal interval! ");
		}
		if(interval->subject > 0) printf("S: %d ", interval->subject);
		printf("\n");
	}
}

void MLPrint::printSpatialInterval(spatial_interval_t* interval) {
	if(interval != NULL) {
		switch(interval->type) {
			case dt_spiA:
//				printf("T: spiA(0x%x, %d) ", interval->type, interval->type);
//				for(int8_t i=0;i<4;i++) {
//					printf("(%d, %d) ", interval->a.x[i], interval->a.y[i]);
//				}
				printf("TODO dt_spiA not supported\n");
				break;
			case dt_spiC:
				printf("T: spiC(0x%x, %d) ", interval->type, interval->type);
				printf("(%d, %d, %d) r=%d ", interval->c.x, interval->c.y, interval->c.z, interval->c.r);
				break;
			case dt_spiV:
				printf("T: spiV(0x%x, %d) ", interval->type, interval->type);
				for(int8_t i=0;i<2;i++) {
					printf("(%d, %d) ", interval->v.x[i], interval->v.y[i]);
				}
				break;
			default:
				printf("T: Unknown spatial interval(%d) ", interval->type);
		}
		if(interval->subject > 0) printf("S: %d ", interval->subject);
		printf("\n");
	}
}

void MLPrint::printObjectAndIntervals(int32_t type, uint8_t buffer[], uint8_t length) {
	ml_object_t mlob;
	spatial_interval_t spint;
	temporal_interval_t tint;
	int32_t index = 0;

	index = MLD_findObjectWithParameters(type, NULL, NULL, buffer, length, &mlob);
	if(index > 0) {
		printf("\t");
		printObject(&mlob);

		if(MLDTemporal_findIntervalWithSubject(index, buffer, length, &tint) > 0) {
			printf("\t");
			MLPrint::printTemporalInterval(&tint);
		}
		else printf("\tTemporal interval not found!\n");

		if(MLDSpatial_findIntervalWithSubject(index, buffer, length, &spint) > 0) {
			printf("\t");
			MLPrint::printSpatialInterval(&spint);
		}
		else printf("\tSpatial interval not found!\n");
	}
}

void MLPrint::printObjectsRecursively(uint8_t subject, uint8_t currentDepth, uint8_t buffer[], uint8_t length) {
	ml_object_t object;
	spatial_interval_t spint;
	temporal_interval_t tint;
	uint8_t j = 0;
	uint8_t i = 1;

	if(currentDepth < MAX_PRINT_DEPTH) {
		MLD_getObjectWithIndex(subject, buffer, length, &object);
		printf("%02u ", subject);
		for(j=0;j<currentDepth;j++) printf("    ");
		if(MLObjects_isSpatialObject(object.type)) {
			if(MLDSpatial_getIntervalWithIndex(subject, buffer, length, &spint) > 0) {
				printSpatialInterval(&spint);
				return; // Actually could have skipped some extra objects that are not part of a standard spint
			}
		}
		if(MLObjects_isTemporalObject(object.type)) {
			if(MLDTemporal_getIntervalWithIndex(subject, buffer, length, &tint) > 0) {
				printTemporalInterval(&tint);
				return; // Actually could have skipped some extra objects that are not part of a standard tint
			}
		}
		MLPrint::printObject(&object);
		while(MLD_getObjectWithIndex(i, buffer, length, &object) > 0) {
			if(object.subject == subject) {
				printObjectsRecursively(i, currentDepth + 1, buffer, length);
			}
			i++;
		}
	}
}

void MLPrint::printObjects(uint8_t buffer[], uint8_t length) {
	ml_object_t object;
	uint8_t i = 1;
	while(MLD_getObjectWithIndex(i, buffer, length, &object) > 0) {
		if(object.subject == 0) {
			printObjectsRecursively(i, 1, buffer, length);
		}
		i++;
	}
}

void MLPrint::printObjectsSequentially(uint8_t buffer[], uint8_t length) {
	ml_object_t object;
	uint8_t i = 1;
	while(MLD_getObjectWithIndex(i, buffer, length, &object) > 0) {
		i++;
		printObject(&object);
	}
}

void MLPrint::printBuffer(uint8_t buffer[], uint8_t length) {
	for(uint8_t i=0;i<length;i++) printf("%02x", buffer[i]);
	printf("\n");
}

MLPrint::MLPrint(MLText* mlt) {
	this->mlt = mlt;
}

MLPrint::~MLPrint() {
	// TODO Auto-generated destructor stub
}
