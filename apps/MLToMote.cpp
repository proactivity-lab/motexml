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
 * @brief Receives messages from serial port and tries to decode them.
 *
 * @author Raido Pahtma
 */
#include "MLEncode.h"
#include "MLDecode.h"
#include "TemporalInterval.h"
#include "SpatialInterval.h"
#include <stdio.h>
#include "SerialMica.h"

#include "MLTestFunctions.cpp"

#define HEADER_LEN 10
#define AM_SOURCE 5555
#define AM_GROUP 0

	SerialMica sm("/dev/ttyUSB0");

	void sendPacket(uint16_t destination, uint8_t handler, uint8_t payload[], uint8_t length) {
		IntArrayClass* arr = new IntArrayClass();
		arr->intArray = new int[HEADER_LEN + length];
		arr->arrayLength = HEADER_LEN + length;

		arr->intArray[0] = 0x44;
		arr->intArray[1] = 0x00;
		arr->intArray[2] = 0x00;
		arr->intArray[3] = (destination >> 8) & 0xFF;
		arr->intArray[4] = destination & 0xFF;
		arr->intArray[5] = (AM_SOURCE >> 8) & 0xFF;
		arr->intArray[6] = AM_SOURCE & 0xFF;
		arr->intArray[7] = length;
		arr->intArray[8] = AM_GROUP;
		arr->intArray[9] = handler;
		for(int i=0;i < length;i++) {
			arr->intArray[i + HEADER_LEN] = payload[i];
		}
		for(int i=0;i < arr->arrayLength;i++) {
			if(arr->intArray[i] < 0x10) printf("0");
			printf("%x ", arr->intArray[i]);
		}
		printf("\n");

		sm.sendData(arr);
	}

	void sendPosInitPacket(uint16_t destination, int32_t x, int32_t y) {
		MLEncode ml;
		spatial_interval_t spint;
		uint8_t buffer[100];
		int32_t value = 0;
		int32_t subject = 0;
		uint8_t size = 0;

		ml.initialize(buffer, 100);
		value = pt_setup;
		subject = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = stp_pos;
		spint.subject = ml.appendObjectWithValues(dt_setup, &subject, &value);
		spint.subjectIsPresent = TRUE;
		spint.type = dt_spiC;
		spint.c.x = x;
		spint.c.y = y;
		spint.c.r = 0;
		ml.appendSpatialInterval(&spint);
		size = ml.finalize();
		sendPacket(destination, 0x08, buffer, size);
	}

	void sendRndInitPacket(uint16_t destination, int32_t x, int32_t y) {
			MLEncode ml;
			spatial_interval_t spint;
			uint8_t buffer[100];
			int32_t value = 0;
			int32_t subject = 0;
			uint8_t size = 0;

			ml.initialize(buffer, 100);
			value = pt_setup;
			subject = ml.appendObjectWithValues(dt_packet, NULL, &value);
			value = stp_pos;
			spint.subject = ml.appendObjectWithValues(dt_setup, &subject, &value);
			spint.subjectIsPresent = TRUE;
			spint.type = dt_spiC;
			spint.c.x = x;
			spint.c.y = y;
			spint.c.r = 0;
			//ml.appendSpatialInterval(&spint);
			size = ml.finalize();
			sendPacket(destination, 0xAA, buffer, size);
	}

	void receiveloop() {
		uint8_t headerLength = 9;
		uint8_t buffer[200];
		IntArrayClass *ic = (IntArrayClass*)sm.getData();
		if(ic != NULL) {
			if(ic->intArray[0] == 0x45) {
				printf("Got a packet!\n");
//				printf("\n");
//				for(int i=0;i < ic->arrayLength - headerLength;i++) {
//					buffer[i] = ic->intArray[i + headerLength];
//				}
//				printBuffer(buffer, ic->arrayLength - 9);
//				printf("\n");
//				decodeTempAndHumAndPrint(buffer, ic->arrayLength - 9);
			}
			else if(ic->intArray[0] == 0x43) printf("Ack ");
			for(int i=0;i < ic->arrayLength;i++) {
				if(ic->intArray[i] < 0x10) printf("0");
				printf("%x ", ic->intArray[i]);
			}
			printf("\n");
			delete ic;
		}
	}

	void locationQuery(uint16_t node) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_dq;

		ml.initialize(buffer, 100);
		ml.appendObjectWithValues(dt_packet, NULL, &value);
		size = ml.finalize();
		sendPacket(node, 0xA, buffer, size);
	}

	int main() {
		uint8_t i = 0;

		LogC::init("logC.log");

		if(sm.openSerialPort() == SUCCESS) {
			printf("Successfully opened serial port.\n");

			while(1) {
				receiveloop();
				sleep(1);
				//if(i < 3) sendPosInitPacket(0xffff, i * 10, 30);
				if(i < 1)  locationQuery(1);
					//sendRndInitPacket(0xffff, i * 10, 30);
				i++;
			}
		}

		return 0;
	}
