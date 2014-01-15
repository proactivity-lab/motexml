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
 * @brief Test app.
 *
 * @author Raido Pahtma
 */
#include "MLE.h"
#include "MLD.h"
#include "MLObjects.h"
#include "MLPrint.h"
#include "MLDTemporal.h"
#include "MLDSpatial.h"
#include <stdio.h>

	int msglevel = 0;

	MLPrint* mlp;

	int main() {
		MLText* mlt = new MLText();
		mlt->initialize("/opt/prolib/dt_types.txt");
		mlp = new MLPrint(mlt);

		ml_encoder_t enc;
		uint8_t buffer[200];
		int32_t size = 0;

		ml_object_t mlob;
		spatial_interval_t spint;
		temporal_interval_t tint;
		int32_t index = 0;

		// Encode 4 really simple objects

		MLE_initialize(&enc, buffer, sizeof(buffer));
		index = MLE_appendObjectWithValues(&enc, 1, 0, &index);
		index = MLE_appendObjectWithValues(&enc, 1, index, &index);
		index = MLE_appendObjectWithValues(&enc, 1, 0, &index);
		index = MLE_appendObjectWithValues(&enc, 1, index, &index);
		size = MLE_finalize(&enc);
		mlp->printObjects(buffer, size);
		printf("Test append object count %d\n",MLD_getObjectCount(buffer, size));
		printf("\n");

		MLE_initialize(&enc, buffer, sizeof(buffer));

		// Encode ------------------------------------------

		mlob.type = dt_tmpC;
		mlob.subject = 0;
		mlob.valueIsPresent = TRUE;
		mlob.value = 23;

		index = MLE_appendObject(&enc, &mlob);
		if(index > 0) {
			printf("Successfully added object %d.\n", index);

			spint.subject = index;
			spint.type = dt_spiC;
			spint.c.r = 100;
			spint.c.x = 200;
			spint.c.y = 300;
			spint.c.z = 0;

			if(MLE_appendSpatialInterval(&enc, &spint) > 0) {
				printf("Successfully added spatial interval.\n");
			}

			tint.subject = index;
			tint.type = dt_timeR;
			tint.elapsed = 100;
			tint.intTime = 2000;

			if(MLE_appendTemporalInterval(&enc, &tint) > 0) {
				printf("Successfully added temporal interval.\n");
			}
		}

		// Encode humidity
		mlob.type = dt_hum;
		mlob.subject = 0;
		mlob.valueIsPresent = TRUE;
		mlob.value = 65;

		index = MLE_appendObject(&enc, &mlob);
		if(index > 0) {
			printf("Successfully added object %d.\n", index);

			spint.subject = index;
			spint.type = dt_spiA;
			spint.a.x[0] = 100;
			spint.a.y[0] = 100;
			spint.a.x[1] = 100;
			spint.a.y[1] = 200;
			spint.a.x[2] = 200;
			spint.a.y[2] = 100;
			spint.a.x[3] = 200;
			spint.a.y[3] = 200;


			if(MLE_appendSpatialInterval(&enc, &spint) > 0) {
				printf("Successfully added spatial interval.\n");
			}

			tint.subject = index;
			tint.type = dt_timeAbs;
			tint.start = 23456789;
			tint.intTime = 60000;

			if(MLE_appendTemporalInterval(&enc, &tint) > 0) {
				printf("Successfully added temporal interval.\n");
			}
		}

		ml_bytebuffer_t bbuf;
		if(MLE_appendByteBufferInit(&enc, &bbuf) > 0) {
			bbuf.subject = 1;
			if(bbuf.length > 25) bbuf.length = 25;
			memset(bbuf.buffer, 0, bbuf.length);
			MLE_appendByteBufferCommit(&enc, &bbuf);
		}

		// Encode humidity
		mlob.type = dt_aId;
		mlob.valueIsPresent = TRUE;
		mlob.value = 5;

		index = MLE_appendObject(&enc, &mlob);
		if(index > 0) {
			printf("Successfully added object %d.\n", index);

			spint.subject = index;
			spint.type = dt_spiV;
			spint.v.x[0] = 100;
			spint.v.y[0] = 100;
			spint.v.x[1] = 200;
			spint.v.y[1] = 200;

			if(MLE_appendSpatialInterval(&enc, &spint) > 0) {
				printf("Successfully added spatial interval.\n");
			}

			tint.subject = index;
			tint.type = dt_timeR;
			tint.start = 45;
			tint.intTime = 15;

			if(MLE_appendTemporalInterval(&enc, &tint) > 0) {
				printf("Successfully added temporal interval.\n");
			}
		}

		size = MLE_finalize(&enc);

		// Print buffer ------------------------------------------

		printf("\n");
		printf("The buffer is %d bytes long.\n", size);
		printf("There are %d objects in the buffer.\n", MLD_getObjectCount(buffer, size));
		printf("\n");
		mlp->printBuffer(buffer, size);
		printf("\n");
		mlp->printObjectsSequentially(buffer, size);
		printf("\n");
		mlp->printObjects(buffer, size);
		printf("\n");

		return 0;
	}
