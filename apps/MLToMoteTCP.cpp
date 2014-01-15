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
 * @brief Receives messages from a udp port and tries to decode them.
 *
 * @author Raido Pahtma
 */
#include "MLEncode.h"
#include "MLDecode.h"
#include "MLObjects.h"
#include "MLText.h"
#include "MLPrint.h"
#include "TemporalInterval.h"
#include "SpatialInterval.h"
#include <stdio.h>
#include "SerialMica.h"
#include "RobotClient.h"
#include "CommHeaders.h"

//#include "MLTestFunctions.cpp"

#define HEADER_LEN 8
#define AM_SOURCE 5678
#define AM_GROUP 0

	MLText mlt;
	MLPrint* mlp = NULL;
	RobotClient rc;
	bool fullOutput = TRUE;

	void sendPacket(uint16_t destination, uint8_t handler, uint8_t payload[], uint8_t length) {
		ByteArrayClass* arr = new ByteArrayClass(HEADER_LEN + length);

		mlp->printObjects(payload, length);

		//arr->array[0] = 0x44;
		arr->array[0] = 0x00;
		arr->array[1] = (destination >> 8) & 0xFF;
		arr->array[2] = destination & 0xFF;
		arr->array[3] = (AM_SOURCE >> 8) & 0xFF;
		arr->array[4] = AM_SOURCE & 0xFF;
		arr->array[5] = length;
		arr->array[6] = AM_GROUP;
		arr->array[7] = handler;
		for(int i=0;i < length;i++) {
			arr->array[i + HEADER_LEN] = payload[i];
		}
		printf("O(%d): ", arr->length);
		for(int i=0;i < arr->length;i++) {
			if(arr->array[i] < 0x10) printf("0");
			printf("%x ", arr->array[i]);
		}
		printf("\n");

		rc.sendData(arr);
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
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void sendInitPacket(uint16_t destination, int32_t param) {
		MLEncode ml;
		uint8_t buffer[100];
		int32_t value = 0;
		int32_t subject = 0;
		uint8_t size = 0;

		ml.initialize(buffer, 100);
		value = pt_setup;
		subject = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = param;
		ml.appendObjectWithValues(dt_setup, &subject, &value);
		size = ml.finalize();
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void sendSensSetupPacket(uint16_t destination, int32_t type, int32_t number, int32_t radius, int32_t period, int32_t validity) {
		MLEncode ml;
		uint8_t buffer[100];
		int32_t value = 0;
		int32_t subject = 0;
		uint8_t size = 0;

		ml.initialize(buffer, 100);
		value = pt_setup;
		subject = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = stp_sens;
		subject = ml.appendObjectWithValues(dt_setup, &subject, &value);
		subject = ml.appendObjectWithValues(type, &subject, NULL); // Append sensor type
		if(number > 0) ml.appendObjectWithValues(dt_dataId, &subject, &number); // Append sensor number to type - optional
		ml.appendObjectWithValues(dt_spiC, &subject, &radius); // Append spiC radius to type
		ml.appendObjectWithValues(dt_period, &subject, &period);
		ml.appendObjectWithValues(dt_intTime, &subject, &validity);
		size = ml.finalize();
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void sendSensDisable(uint16_t destination, int32_t type, int32_t number) {
		MLEncode ml;
		uint8_t buffer[100];
		int32_t value = 0;
		int32_t subject = 0;
		uint8_t size = 0;

		ml.initialize(buffer, 100);
		value = pt_setup;
		subject = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = stp_sensCnc;
		subject = ml.appendObjectWithValues(dt_setup, &subject, &value);
		subject = ml.appendObjectWithValues(type, &subject, NULL); // Append sensor type
		if(number > 0) ml.appendObjectWithValues(dt_dataId, &subject, &number); // Append sensor number to type - optional
		size = ml.finalize();
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void sendSensStpQuery(uint16_t destination) {
		MLEncode ml;
		uint8_t buffer[100];
		int32_t value = 0;
		int32_t subject = 0;
		uint8_t size = 0;

		ml.initialize(buffer, 100);
		value = pt_dq;
		subject = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = stp_sens;
		ml.appendObjectWithValues(dt_setup, &subject, &value);
		value = AM_SOURCE;
		ml.appendObjectWithValues(dt_aId, &subject, &value);

		size = ml.finalize();
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void proccessMLPacket(uint16_t source, uint16_t destination, uint8_t am, uint8_t* buffer, uint8_t length) {
		int32_t ndex = 0;
		ml_object_t object;
		int32_t subject = 0;

		printf("SRC: %i, DST: %i, AM: 0x%x\n",source, destination, am);
		ndex = MLDecode::findObjectWithParameters(dt_packet, NULL, NULL, buffer, length, &object);
		if((ndex > 0) && object.valueIsPresent) {
			switch(object.value) {
				case pt_posAnc:
					printf("Positioning announcement\n");
					mlp->printObjects(buffer, length);
					break;
				case pt_posDq:
					printf("Positioning data query\n");
					mlp->printObjects(buffer, length);
					break;
				case pt_posDqr:
					printf("Positioning data query response\n");
					mlp->printObjects(buffer, length);
					break;
				case pt_posFailed:
					printf("Positioning failed\n");
					mlp->printObjects(buffer, length);
					break;
				case pt_posFinal:
					printf("Positioning final\n");
					mlp->printObjects(buffer, length);
					break;
				case pt_posRanging:
					printf("Positioning ranging\n");
					mlp->printObjects(buffer, length);
					break;
				case pt_posResult:
					printf("Positioning result\n");
					mlp->printObjects(buffer, length);
					break;
				case pt_dqr:
					printf("Query response from %d: \n", source);
					mlp->printObjects(buffer, length);
					break;
				case pt_dq:
					printf("Query by %d: \n", source);
					mlp->printObjects(buffer, length);
					break;
				case pt_subsAnc:
				case pt_subsEnd:
					if(object.value == pt_subsAnc) printf("Subscription announcement from %d: ", source);
					else printf("Subscription ended on mote %d: ", source);
					if(MLDecode::findObjectWithParameters(dt_aId, &ndex, NULL, buffer, length, &object) > 0) {
						printf("client: %d ", object.value);
					}
					if(MLDecode::findObjectWithParameters(dt_dataId, &ndex, NULL, buffer, length, &object) > 0) {
						printf("id: %d ", object.value);
					}
					if(MLDecode::findObjectWithParameters(dt_count, &ndex, NULL, buffer, length, &object) > 0) {
						printf("free: %d ", object.value);
					}
					printf("\n");
					break;
				case pt_subs:
					printf("Subscription from %d: \n", source);
					mlp->printObjects(buffer, length);
					break;
				case pt_subsCnc:
					printf("Subscription cancellation from %d: \n", source);
					mlp->printObjects(buffer, length);
					break;
				case pt_debug:
					printf("Debug packet from %d ", source);
					if((subject = MLDecode::findObjectWithParameters(dt_setup, &ndex, NULL, buffer, length, &object)) > 0) {
						if(object.valueIsPresent) printf("for system %d(0x%x) ", object.value, object.value);
					}
					if(MLDecode::findObjectWithParameters(dt_debug, &subject, NULL, buffer, length, &object) > 0) {
						if(object.valueIsPresent) printf("with value %d(0x%x) ", object.value, object.value);
					}
					printf("\n");
					break;
			}
		}
		else mlp->printObjects(buffer, length);
	}

	void receive(void* receiver, ByteArrayClass *ic) {
		uint8_t length = 0;
		uint8_t *buffer = NULL;
		uint16_t source = 0;
		uint16_t destination = 0;
		uint8_t am = 0;

		if(ic != NULL) {
			if(ic->length > HEADER_LEN) {
				buffer = new uint8_t[ic->length];
				length = ic->length - HEADER_LEN;
				source = ((uint16_t)ic->array[3] << 8) + ic->array[4];
				destination = ((uint16_t)ic->array[1] << 8) + ic->array[2];
				am = ic->array[7];

				if(fullOutput) printf("I(%d): ", ic->length);
				for(int i=0;i < ic->length;i++) {
					if(fullOutput) {
						if(ic->array[i] < 0x10) printf("0");
						printf("%x ", ic->array[i]);
					}
					if(i >= HEADER_LEN) buffer[i - HEADER_LEN] = ic->array[i]; // Copy contents
				}
				if(fullOutput) printf("\n");

				proccessMLPacket(source, destination, am, buffer, length);
				printf("--------------------------------------------------------------------------------\n");

				delete ic;
				delete [] buffer;
			}
			else printf("Received a strange short packet!\n");
		}
	}

	void sendDBQuery(int32_t type, int32_t x, int32_t y, int32_t r, int32_t count) {
		MLEncode ml;
		spatial_interval_t spint;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_dq;
		int32_t ndex = 0;

		ml.initialize(buffer, 100);
		ndex = ml.appendObjectWithValues(dt_packet, NULL, &value);
		ml.appendObjectWithValues(dt_count, &ndex, &count);
		spint.subject = ml.appendObjectWithValues(type, NULL, NULL);
		spint.subjectIsPresent = TRUE;
		spint.type = dt_spiC;
		spint.c.x = x;
		spint.c.y = y;
		spint.c.r = r;
		ml.appendSpatialInterval(&spint);
		size = ml.finalize();
		sendPacket(0xFFFF, DIST_DBC_HEADER, buffer, size);
	}

	void doTimeQuery(int32_t type, int32_t elapsed, int32_t end, int32_t count) {
		MLEncode ml;
		temporal_interval_t tint;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_dq;
		int32_t ndex = 0;

		ml.initialize(buffer, 100);
		ndex = ml.appendObjectWithValues(dt_packet, NULL, &value);
		ml.appendObjectWithValues(dt_count, &ndex, &count);
		tint.subject = ml.appendObjectWithValues(type, NULL, NULL);
		tint.subjectIsPresent = TRUE;
		tint.type = dt_timeR;
		tint.elapsed = elapsed;
		tint.intTime = end;
		ml.appendTemporalInterval(&tint);
		size = ml.finalize();
		sendPacket(0xFFFF, DIST_DBC_HEADER, buffer, size);
	}

	void locationQuery(uint16_t node) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_dq;
		int32_t ndex = 0;

		ml.initialize(buffer, 100);
		ndex = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = AM_SOURCE;
		//ml.appendObjectWithValues(dt_aId, &ndex, &value);
		size = ml.finalize();
		sendPacket(node, NODE_LOCATION_HEADER, buffer, size);
	}

	void locationSubscription(uint16_t node) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_subs;
		int32_t ndex = 0;

		ml.initialize(buffer, 100);
		ndex = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = AM_SOURCE;
		ml.appendObjectWithValues(dt_aId, &ndex, &value);
		size = ml.finalize();
		sendPacket(node, NODE_LOCATION_HEADER, buffer, size);
	}

	void rssiTableQuery(uint16_t node, int32_t type) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t ndex = 0;
		int32_t value = pt_dq;

		ml.initialize(buffer, 100);
		ndex = ml.appendObjectWithValues(dt_packet, NULL, &value);
		ml.appendObjectWithValues(dt_aType, &ndex, &type);

		size = ml.finalize();
		sendPacket(node, CABP_RADIUS_CONF, buffer, size);
	}

	void setRssiOffset(uint16_t node, int32_t type, int32_t str, int32_t offset) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t ndex = 0;
		int32_t value = pt_dp;

		ml.initialize(buffer, 100);
		ndex = ml.appendObjectWithValues(dt_packet, NULL, &value);
		ndex = ml.appendObjectWithValues(dt_aType, &ndex, &type);
		ndex = ml.appendObjectWithValues(dt_power, &ndex, &str);
		ml.appendObjectWithValues(dt_rssi, &ndex, &offset);

		size = ml.finalize();
		sendPacket(node, CABP_RADIUS_CONF, buffer, size);
	}

	void subscribeToLocation(int32_t type, int32_t id, int32_t period, int32_t duration, int32_t x, int32_t y, int32_t r) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		int32_t index = 0;
		spatial_interval_t spint;

		ml.initialize(buffer, 100);
		value = pt_subs;
		index = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = AM_SOURCE;
		ml.appendObjectWithValues(dt_aId, &index, &value);
		ml.appendObjectWithValues(dt_dataId, &index, &id);
		ml.appendObjectWithValues(dt_period, &index, &period);
		ml.appendObjectWithValues(dt_intTime, &index, &duration);

		spint.subject = ml.appendObjectWithValues(type, NULL, NULL);
		spint.subjectIsPresent = TRUE;
		spint.type = dt_spiC;
		spint.c.x = x;
		spint.c.y= y;
		spint.c.r = r;
		ml.appendSpatialInterval(&spint);

		size = ml.finalize();
		sendPacket(0xFFFF, DIST_SUBS_HEADER, buffer, size);
	}

	void subscribeDynamic(int32_t type, int32_t id, int32_t period, int32_t duration, int32_t radius, int32_t node) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		int32_t index = 0;
		ml_object_t object;

		ml.initialize(buffer, 100);
		value = pt_subs;
		index = ml.appendObjectWithValues(dt_packet, NULL, &value);
		value = AM_SOURCE;
		ml.appendObjectWithValues(dt_aId, &index, &value);
		ml.appendObjectWithValues(dt_dataId, &index, &id);
		ml.appendObjectWithValues(dt_period, &index, &period);
		ml.appendObjectWithValues(dt_intTime, &index, &duration);

		object.subject = ml.appendObjectWithValues(type, NULL, NULL);
		object.subjectIsPresent = TRUE;
		object.type = dt_spiD;
		object.valueIsPresent = TRUE;
		object.value = radius;

		object.subject = ml.appendObject(&object);
		object.subjectIsPresent = TRUE;
		object.type = dt_aId;
		object.valueIsPresent = TRUE;
		object.value = node;
		ml.appendObject(&object);

		size = ml.finalize();
		sendPacket(0xFFFF, DIST_SUBS_HEADER, buffer, size);
	}

	void cancelSubscription(int32_t client, int32_t id) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		int32_t index = 0;

		ml.initialize(buffer, 100);
		value = pt_subsCnc;
		index = ml.appendObjectWithValues(dt_packet, NULL, &value);
		ml.appendObjectWithValues(dt_aId, &index, &client);
		ml.appendObjectWithValues(dt_dataId, &index, &id);

		size = ml.finalize();
		sendPacket(0xFFFF, DIST_SUBS_HEADER, buffer, size);
	}

	void setPosProxy(int32_t destination, int32_t stats, int32_t proxy) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		int32_t index = 0;

		ml.initialize(buffer, 100);
		value = pt_dp;
		index = ml.appendObjectWithValues(dt_packet, NULL, &value);
		index = ml.appendObjectWithValues(dt_setup, &index, NULL);
		ml.appendObjectWithValues(dt_rssi, &index, &stats);
		ml.appendObjectWithValues(dt_loc, &index, &proxy);

		size = ml.finalize();
		sendPacket(destination, POS_ANALYSIS_CONTROL_HEADER, buffer, size);
	}

	void getPosProxy(int32_t destination) {
		MLEncode ml;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;

		ml.initialize(buffer, 100);
		value = pt_dq;
		ml.appendObjectWithValues(dt_packet, NULL, &value);

		size = ml.finalize();
		sendPacket(destination, POS_ANALYSIS_CONTROL_HEADER, buffer, size);
	}

	void printUse() {
		printf("Use:\n");
		printf("\tdebug\n");
		printf("\tlocsetup\n");
		printf("\tsenssetup\n");
		printf("\tsenscnc\n");
		printf("\tsensstpquery\n");
		printf("\trquery\n");
		printf("\ttimequery\n");
		printf("\tlocation\n");
		printf("\tlocsubs\n");
		printf("\tsubsarea\n");
		printf("\tsubsdynamic\n");
		printf("\tsubscnc\n");
		printf("\tlisten\n");
		printf("\trssiquery\n");
		printf("\tsetrssioffset\n");
		printf("\tgetposproxy\n");
		printf("\tsetposproxy\n");
	}

	error_t processInput(int argc, const char* argv[], int argi[], int maxargs) {
		if(argc != maxargs) return FAIL;
		for(int i=2;i<maxargs;i++) {
			if((argv[i][0] == '0') && (argv[i][1] == 'x')) sscanf(&argv[i][2], "%x", &argi[i]);
			else if((argv[i][0] == 'd') && (argv[i][1] == 't')) return FAIL;
			else argi[i] = atoi(argv[i]);
		}
		return SUCCESS;
	}

	int main(int argc, const char* argv[]) {
		LogC::init("/tmp/logC.log");
		int sleeplen = 0;
		int argi[argc];

		rc.setListener(&receive, NULL);
		mlt.initialize("dt_types.txt");
		mlp = new MLPrint(&mlt);

		if(rc.openConnection("127.0.0.1", 9002) == SUCCESS) {
			printf("Successfully opened port.\n");

			if(argc > 1) {
				printf("%s\n", argv[1]);
				sleeplen = 90;
				if(strcmp(argv[1], "debug") == 0) {
					mlt.printAllValues();
				}
				// RSSI offsets
				else if(strcmp(argv[1], "rssiquery") == 0) {
					if(processInput(argc, argv, argi, 4) == SUCCESS) rssiTableQuery(argi[2], argi[3]);
					else printf("node type\n");
				}
				else if(strcmp(argv[1], "setrssioffset") == 0) {
					if(processInput(argc, argv, argi, 6) == SUCCESS) setRssiOffset(argi[2], argi[3], argi[4], argi[5]);
					else printf("node type str offset\n");
				}
				// Sensor setup
				else if(strcmp(argv[1], "senssetup") == 0) {
					if(processInput(argc, argv, argi, 8) == SUCCESS) sendSensSetupPacket(argi[2], argi[3], argi[4], argi[5], argi[6], argi[7]);
					else printf("node type number radius period validity\n");
				}
				else if(strcmp(argv[1], "senscnc") == 0) {
					if(processInput(argc, argv, argi, 5) == SUCCESS) sendSensDisable(argi[2], argi[3], argi[4]);
					else printf("node type number\n");
				}
				else if(strcmp(argv[1], "sensstpquery") == 0) {
					if(processInput(argc, argv, argi, 3) == SUCCESS) sendSensStpQuery(argi[2]);
					else printf("node\n");
				}
				// Database queries
				else if(strcmp(argv[1], "rquery") == 0) {
					if(processInput(argc, argv, argi, 7) == SUCCESS) sendDBQuery(argi[2], argi[3], argi[4], argi[5], argi[6]);
					else printf("type x y r count\n");
				}
				else if(strcmp(argv[1], "timequery") == 0) {
					if(processInput(argc, argv, argi, 6) == SUCCESS) doTimeQuery(argi[2], argi[3], argi[4], argi[5]);
					else printf("type elapsed duration count\n");
				}
				// Location
				else if(strcmp(argv[1], "locsetup") == 0) {
					if(processInput(argc, argv, argi, 5) == SUCCESS) sendPosInitPacket(argi[2], argi[3], argi[4]);
					else printf("node x y\n");
				}
				else if(strcmp(argv[1], "location") == 0) {
					if(processInput(argc, argv, argi, 3) == SUCCESS) locationQuery(argi[2]);
					else printf("node?\n");
				}
				else if(strcmp(argv[1], "locsubs") == 0) {
					if(processInput(argc, argv, argi, 3) == SUCCESS) locationSubscription(argi[2]);
					else printf("node?\n");
				}
				// Subscriptions
				else if(strcmp(argv[1], "subsarea") == 0) {
					if(processInput(argc, argv, argi, 9) == SUCCESS) subscribeToLocation(argi[2], argi[3], argi[4], argi[5], argi[6], argi[7], argi[8]);
					else printf("type id period duration x y r\n");
				}
				else if(strcmp(argv[1], "subsdynamic") == 0) {
					if(processInput(argc, argv, argi, 8) == SUCCESS) subscribeDynamic(argi[2], argi[3], argi[4], argi[5], argi[6], argi[7]);
					else printf("type id period duration radius node\n");
				}
				else if(strcmp(argv[1], "subscnc") == 0) {
					if(processInput(argc, argv, argi, 4) == SUCCESS) cancelSubscription(argi[2], argi[3]);
					else printf("client id\n");
				}
				else if(strcmp(argv[1], "setposproxy") == 0) {
					if(processInput(argc, argv, argi, 5) == SUCCESS) setPosProxy(argi[2], argi[3], argi[4]);
					else printf("destination stats proxy\n");
				}
				else if(strcmp(argv[1], "getposproxy") == 0) {
					if(processInput(argc, argv, argi, 3) == SUCCESS) getPosProxy(argi[2]);
					else printf("destination\n");
				}
				// Listen
				else if(strcmp(argv[1], "listen") == 0) {
					if(argc >= 3) {
						sleeplen = atoi(argv[2]);
						printf("(%d)\n", sleeplen);
					}
					else printf("listen time <full>\n");
					if(argc >= 4) {
						if(strcmp(argv[3], "full") == 0) fullOutput = TRUE;
					}
				} else printUse();
			} else printUse();
			sleep(sleeplen);
		}

		return 0;
	}
