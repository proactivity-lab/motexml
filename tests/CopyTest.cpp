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

int main(int argc, const char* argv[]) {
	uint8_t buf[102];
	ml_encoder_t enc;
	uint8_t size;
	ml_object_t ob;
	uint8_t obuffer[] = {0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

	if(argc <= 1) {
		printf("Usage: %s index_of_element_to_copy\n", argv[0]);
		return 1;
	}

	uint8_t cindex = atoi(argv[1]);

	MLE_initialize(&enc, buf, sizeof(buf));
	uint8_t ndex = MLE_appendO(&enc, 0x11);
	ob.subject = MLE_appendOSV(&enc, 0x12, ndex, 1);
	ob.valueIsPresent = TRUE;
	ob.value = 0xab;
	ob.buffer = obuffer;
	ob.bufferLength = sizeof(obuffer);
	ob.type = 0x13;
	MLE_appendObject(&enc, &ob);
	MLE_appendOSV(&enc, 0x12, ndex, 2);
	uint8_t ndex2 = MLE_appendOSV(&enc, 0x12, ndex, 3);
	uint8_t ndex3 = MLE_appendOSV(&enc, 0x13, ndex2, 1);
	MLE_appendOSV(&enc, 0x13, ndex2, 2);
	MLE_appendOSV(&enc, 0x13, ndex2, 3);
	MLE_appendOSV(&enc, 0x14, ndex3, 1);
	MLE_appendOSV(&enc, 0x14, ndex3, 2);
	uint8_t ndex4 = MLE_appendOSV(&enc, 0x14, ndex3, 3);
	uint8_t ndex5 = MLE_appendOSV(&enc, 0x15, ndex4, 1);
	MLE_appendOSV(&enc, 0x15, ndex4, 2);
	MLE_appendOSV(&enc, 0x16, ndex5, 1);
	MLE_appendOV(&enc, 0x11, 2);

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

	ml_encoder_t enc2;
	uint8_t buf2[102];
	uint8_t size2;
	MLE_initialize(&enc2, buf2, sizeof(buf2));
	ndex = MLE_appendO(&enc2, 0xaa);

	printf("\nCopying:\n");
	printf("Done(%u)\n", MLI_icopy(cindex, buf, size, ndex, &enc2));
	size2 = MLE_finalize(&enc2);

	printf("\nObjects:\n");
	mlp->printObjects(buf2, size2);

	printf("\nIterated:\n");
	MLI_initialize(&iter, buf2, size2);
	while(MLI_next(&iter, &object) > 0) {
		mlp->printObject(&object);
	}

	return 0;
}
