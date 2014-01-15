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
	uint8_t ndex, ndex2;

	MLE_initialize(&enc, buf, sizeof(buf));
	ndex = MLE_appendO(&enc, dt_packet);
	MLE_appendOSV(&enc, dt_aId, ndex, 3);
	ndex2 = MLE_appendOSV(&enc, dt_dataId, ndex, 4);
	MLE_appendOSV(&enc, dt_aType, ndex2, 1234);
	MLE_appendOSV(&enc, dt_acc, ndex, 23);
	MLE_appendOV(&enc, dt_airflow, 100);

	size = MLE_finalize(&enc);

	MLText mlt;
	mlt.initialize("/opt/prolib/dt_types.txt");
	MLPrint* mlp = new MLPrint(&mlt);

	printf("Buffer:\n");
	mlp->printBuffer(buf, size);
	printf("\nObjects:\n");
	mlp->printObjects(buf, size);

	ml_iterator_t iter;
	ml_object_t object;
	MLI_initialize(&iter, buf, size);
	printf("\nIterated:\n");
	while(MLI_next(&iter, &object) > 0) {
		mlp->printObject(&object);
	}

	MLI_reset(&iter);
	printf("\nIterated subject == 1:\n");
	while(MLI_nextWithSubject(&iter, 1, &object) > 0) {
		mlp->printObject(&object);
	}
}
