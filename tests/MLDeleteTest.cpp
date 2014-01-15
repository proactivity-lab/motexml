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
#include "MLFormat.h"
#include "MLI.h"
#include "MLPrint.h"
#include "MLText.h"
#include "debug.h"

int msglevel = 0;

int main() {
	uint8_t buf[102];
	ml_encoder_t enc;
	uint8_t size;
	uint8_t ndex, ndex2, ndex3, ndex4, ndex5, ndex6;

	uint8_t obuffer[] = {0xa,0xb,0xc,0xd,0xe};
	ml_object_t object;
	object.type = dt_packet;
	object.subject = 0;
	object.valueIsPresent = FALSE;
	object.buffer = obuffer;
	object.bufferLength = sizeof(obuffer);

	MLE_initialize(&enc, buf, sizeof(buf));
	ndex = MLE_appendObject(&enc, &object);
	ndex2 = MLE_appendOSV(&enc, dt_aId, ndex, 3);
	ndex3 = MLE_appendOSV(&enc, dt_aId, ndex2, 4);
	MLE_appendOSV(&enc, dt_aId, ndex3, 5);
	ndex2 = MLE_appendOSV(&enc, dt_dataId, ndex, 4);
	MLE_appendOSV(&enc, dt_aType, ndex2, 1234);
	ndex6 = MLE_appendOSV(&enc, dt_acc, ndex, 23);
	MLE_appendOSV(&enc, dt_acc, ndex6, 24);
	ndex3 = MLE_appendOV(&enc, dt_airflow, 100);
	ndex4 = MLE_appendOSV(&enc, dt_airflow, ndex3, 3);
	ndex5 = MLE_appendOSV(&enc, dt_airflow, ndex4, 4);
	MLE_appendOSV(&enc, dt_airflow, ndex5, 200);

	size = MLE_finalize(&enc);

	MLText mlt;
	mlt.initialize("/opt/prolib/dt_types.txt");
	MLPrint* mlp = new MLPrint(&mlt);

	printf("Buffer(%d):\n", size);
	mlp->printBuffer(buf, size);
	printf("Objects:\n");
	mlp->printObjects(buf, size);

	printf("\n");
	printf("Reinitialize & append\n");
	MLE_reinitialize(&enc, buf, size, sizeof(buf));
	MLE_appendOSV(&enc, dt_absTime, ndex3, 200);

	printf("Buffer(%d):\n", MLE_spaceUsed(&enc));
	mlp->printBuffer(buf, MLE_spaceUsed(&enc));
	printf("Objects:\n");
	mlp->printObjects(buf, MLE_spaceUsed(&enc));

	printf("\n");
	printf("Delete %d\n", MLE_deleteObject(&enc, ndex2));
	printf("Buffer(%d):\n", MLE_spaceUsed(&enc));
	mlp->printBuffer(buf, MLE_spaceUsed(&enc));
	printf("\nObjects:\n");
	mlp->printObjects(buf, MLE_spaceUsed(&enc));

	printf("\n");
	printf("Delete %d\n", MLE_deleteObject(&enc, enc.currentPosition));
	printf("Buffer(%d):\n", MLE_spaceUsed(&enc));
	mlp->printBuffer(buf, MLE_spaceUsed(&enc));
	printf("Objects:\n");
	mlp->printObjects(buf, MLE_spaceUsed(&enc));

	printf("\n");
	printf("Delete %d\n", MLE_deleteObject(&enc, 1));
	printf("Buffer(%d):\n", MLE_spaceUsed(&enc));
	mlp->printBuffer(buf, MLE_spaceUsed(&enc));
	printf("Objects:\n");
	mlp->printObjects(buf, MLE_spaceUsed(&enc));
}
