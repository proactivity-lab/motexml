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
#include "MLText.h"
#include "MLPrint.h"
#include "MLDTemporal.h"
#include "MLDSpatial.h"
#include <stdio.h>
#include <unistd.h>
#include "ActiveMessage.h"
#include "CommHeaders.h"

#define AM_BROADCAST_ADDR 0xFFFF
#define AM_SOURCE 0x64
#define AM_GROUP 0

	MLText mlt;
	MLPrint* mlp = NULL;
	bool fullOutput = TRUE;
	ActiveMessage* am = NULL;

	void sendPacket(am_addr_t destination, uint8_t handler, uint8_t payload[], uint8_t length) {
		MessageT* msg = new MessageT();
		uint8_t* msgpayload = (uint8_t*)msg->getPayload(length);
		char* msgtext;

		if(msgpayload != NULL) {
			msg->setType(handler);
			memcpy(msgpayload, payload, length);
			mlp->printObjects(payload, length);
			if(am->send(destination, msg, length) != SUCCESS) {
				printf("Sending message failed!\n");
			}
			else {
				msgtext = msg->toString();
				printf("O(%d): %s\n", msg->getTotalLength(), msgtext);
				delete [] msgtext;
			}
		}
	}

	void sendPosInitPacket(uint16_t destination, int32_t x, int32_t y) {
		ml_encoder_t enc;
		spatial_interval_t spint;
		uint8_t buffer[100];
		uint8_t subject = 0;
		uint8_t size = 0;

		MLE_initialize(&enc, buffer, 100);
		subject = MLE_appendOV(&enc, dt_packet, pt_setup);
		spint.subject = MLE_appendOSV(&enc, dt_setup, subject, stp_pos);
		spint.type = dt_spiC;
		spint.c.x = x;
		spint.c.y = y;
		spint.c.r = 0;
		MLE_appendSpatialInterval(&enc, &spint);
		size = MLE_finalize(&enc);
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void sendInitPacket(uint16_t destination, int32_t param) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t subject = 0;
		uint8_t size = 0;

		MLE_initialize(&enc, buffer, 100);
		subject = MLE_appendOV(&enc, dt_packet, pt_setup);
		MLE_appendOSV(&enc, dt_setup, subject, param);
		size = MLE_finalize(&enc);
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void sendSensSetupPacket(uint16_t destination, int32_t type, int32_t number, int32_t radius, int32_t period, int32_t validity) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		int32_t value = 0;
		uint8_t subject = 0;
		uint8_t size = 0;

		MLE_initialize(&enc, buffer, 100);
		subject = MLE_appendOV(&enc, dt_packet, pt_setup);
		value = stp_sens;
		subject = MLE_appendObjectWithValues(&enc, dt_setup, subject, &value);
		subject = MLE_appendObjectWithValues(&enc, type, subject, NULL); // Append sensor type
		if(number > 0) MLE_appendObjectWithValues(&enc, dt_dataId, subject, &number); // Append sensor number to type - optional
		MLE_appendObjectWithValues(&enc, dt_spiC, subject, &radius); // Append spiC radius to type
		MLE_appendObjectWithValues(&enc, dt_period, subject, &period);
		MLE_appendObjectWithValues(&enc, dt_intTime, subject, &validity);
		size = MLE_finalize(&enc);
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void sendSensDisable(uint16_t destination, int32_t type, int32_t number) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		int32_t value = 0;
		uint8_t subject = 0;
		uint8_t size = 0;

		MLE_initialize(&enc, buffer, 100);
		subject = MLE_appendOV(&enc, dt_packet, pt_setup);
		value = stp_sensCnc;
		subject = MLE_appendObjectWithValues(&enc, dt_setup, subject, &value);
		subject = MLE_appendObjectWithValues(&enc, type, subject, NULL); // Append sensor type
		if(number > 0) MLE_appendObjectWithValues(&enc, dt_dataId, subject, &number); // Append sensor number to type - optional
		size = MLE_finalize(&enc);
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void sendSensStpQuery(uint16_t destination) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		int32_t value = 0;
		uint8_t subject = 0;
		uint8_t size = 0;

		MLE_initialize(&enc, buffer, 100);
		subject = MLE_appendOV(&enc, dt_packet, pt_dq);
		value = stp_sens;
		MLE_appendObjectWithValues(&enc, dt_setup, subject, &value);
		value = AM_SOURCE;
		MLE_appendObjectWithValues(&enc, dt_aId, subject, &value);

		size = MLE_finalize(&enc);
		sendPacket(destination, LANODE_SETUP_HEADER, buffer, size);
	}

	void proccessMLPacket(uint16_t source, uint16_t destination, uint8_t am, uint8_t* buffer, uint8_t length) {
		uint8_t ndex = 0;
		ml_object_t object;
		uint8_t subject = 0;

		printf("SRC: %i, DST: %i, AM: 0x%x\n",source, destination, am);
		ndex = MLD_findObjectWithParameters(dt_packet, NULL, NULL, buffer, length, &object);
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
					if(MLD_findObjectWithParameters(dt_aId, &ndex, NULL, buffer, length, &object) > 0) {
						printf("client: %d ", object.value);
					}
					if(MLD_findObjectWithParameters(dt_dataId, &ndex, NULL, buffer, length, &object) > 0) {
						printf("id: %d ", object.value);
					}
					printf("\n");
					mlp->printObjects(buffer, length);
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
					if((subject = MLD_findObjectWithParameters(dt_setup, &ndex, NULL, buffer, length, &object)) > 0) {
						if(object.valueIsPresent) printf("for system %d(0x%x) ", object.value, object.value);
					}
					if(MLD_findObjectWithParameters(dt_debug, &subject, NULL, buffer, length, &object) > 0) {
						if(object.valueIsPresent) printf("with value %d(0x%x) ", object.value, object.value);
					}
					printf("\n");
					break;
			}
		}
		else mlp->printObjects(buffer, length);

	}

	MessageT* receive(void* object, MessageT* msg, void* payload, uint8_t length) {
		char* msgtext;

		if(fullOutput) {
			msgtext = msg->toString();
			printf("I(%d): %s\n", msg->getTotalLength(), msgtext);
			delete [] msgtext;
		}

		proccessMLPacket(msg->source(), msg->destination(), msg->type(), (uint8_t*)payload, length);
		printf("--------------------------------------------------------------------------------\n");
		return msg;
	}

	void sendDBQuery(int32_t type, int32_t x, int32_t y, int32_t r, int32_t count) {
		ml_encoder_t enc;
		spatial_interval_t spint;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_dq;
		int32_t ndex = 0;

		MLE_initialize(&enc, buffer, 100);
		ndex = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		MLE_appendObjectWithValues(&enc, dt_count, ndex, &count);
		spint.subject = MLE_appendObjectWithValues(&enc, type, 0, NULL);
		spint.type = dt_spiC;
		spint.c.x = x;
		spint.c.y = y;
		spint.c.r = r;
		MLE_appendSpatialInterval(&enc, &spint);
		size = MLE_finalize(&enc);
		sendPacket(AM_BROADCAST_ADDR, DIST_DBC_HEADER, buffer, size);
	}

	void doTimeQuery(int32_t type, int32_t elapsed, int32_t end, int32_t count) {
		ml_encoder_t enc;
		temporal_interval_t tint;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_dq;
		int32_t ndex = 0;

		MLE_initialize(&enc, buffer, 100);
		ndex = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		MLE_appendObjectWithValues(&enc, dt_count, ndex, &count);
		tint.subject = MLE_appendObjectWithValues(&enc, type, 0, NULL);
		tint.type = dt_timeR;
		tint.elapsed = elapsed;
		tint.intTime = end;
		MLE_appendTemporalInterval(&enc, &tint);
		size = MLE_finalize(&enc);
		sendPacket(AM_BROADCAST_ADDR, DIST_DBC_HEADER, buffer, size);
	}

	void locationQuery(uint16_t node) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_dq;
		int32_t ndex = 0;

		MLE_initialize(&enc, buffer, 100);
		ndex = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		value = AM_SOURCE;
		//MLE_appendObjectWithValues(&enc, dt_aId, &ndex, &value);
		size = MLE_finalize(&enc);
		sendPacket(node, NODE_LOCATION_HEADER, buffer, size);
	}

	void locationSubscription(uint16_t node) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = pt_subs;
		int32_t ndex = 0;

		MLE_initialize(&enc, buffer, 100);
		ndex = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		value = AM_SOURCE;
		MLE_appendObjectWithValues(&enc, dt_aId, ndex, &value);
		size = MLE_finalize(&enc);
		sendPacket(node, NODE_LOCATION_HEADER, buffer, size);
	}

	void rssiTableQuery(uint16_t node, int32_t type) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t ndex = 0;
		int32_t value = pt_dq;

		MLE_initialize(&enc, buffer, 100);
		ndex = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		MLE_appendObjectWithValues(&enc, dt_aType, ndex, &type);

		size = MLE_finalize(&enc);
		sendPacket(node, CABP_RADIUS_CONF, buffer, size);
	}

	void setRssiOffset(uint16_t node, int32_t type, int32_t str, int32_t offset) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t ndex = 0;
		int32_t value = pt_dp;

		MLE_initialize(&enc, buffer, 100);
		ndex = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		ndex = MLE_appendObjectWithValues(&enc, dt_aType, ndex, &type);
		ndex = MLE_appendObjectWithValues(&enc, dt_power, ndex, &str);
		MLE_appendObjectWithValues(&enc, dt_rssi, ndex, &offset);

		size = MLE_finalize(&enc);
		sendPacket(node, CABP_RADIUS_CONF, buffer, size);
	}

	void subscribeToLocation(int32_t type, int32_t id, int32_t period, int32_t duration, int32_t x, int32_t y, int32_t r) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		int32_t index = 0;
		spatial_interval_t spint;

		MLE_initialize(&enc, buffer, 100);
		value = pt_subs;
		index = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		value = AM_SOURCE;
		MLE_appendObjectWithValues(&enc, dt_aId, index, &value);
		MLE_appendObjectWithValues(&enc, dt_dataId, index, &id);
		MLE_appendObjectWithValues(&enc, dt_period, index, &period);
		MLE_appendObjectWithValues(&enc, dt_intTime, index, &duration);

		spint.subject = MLE_appendObjectWithValues(&enc, type, 0, NULL);
		spint.type = dt_spiC;
		spint.c.x = x;
		spint.c.y = y;
		spint.c.z = 0;
		spint.c.r = r;
		MLE_appendSpatialInterval(&enc, &spint);

		size = MLE_finalize(&enc);
		sendPacket(AM_BROADCAST_ADDR, DIST_SUBS_HEADER, buffer, size);
	}

	void subscribeDynamic(int32_t type, int32_t id, int32_t period, int32_t duration, int32_t radius, int32_t node) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		int32_t index = 0;
		ml_object_t object;

		MLE_initialize(&enc, buffer, 100);
		value = pt_subs;
		index = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		value = AM_SOURCE;
		MLE_appendObjectWithValues(&enc, dt_aId, index, &value);
		MLE_appendObjectWithValues(&enc, dt_dataId, index, &id);
		MLE_appendObjectWithValues(&enc, dt_period, index, &period);
		MLE_appendObjectWithValues(&enc, dt_intTime, index, &duration);

		object.subject = MLE_appendObjectWithValues(&enc, type, 0, NULL);
		object.type = dt_spiD;
		object.valueIsPresent = TRUE;
		object.value = radius;

		object.subject = MLE_appendObject(&enc, &object);
		object.type = dt_aId;
		object.valueIsPresent = TRUE;
		object.value = node;
		MLE_appendObject(&enc, &object);

		size = MLE_finalize(&enc);
		sendPacket(AM_BROADCAST_ADDR, DIST_SUBS_HEADER, buffer, size);
	}

	void cancelSubscription(int32_t client, int32_t id) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		int32_t index = 0;

		MLE_initialize(&enc, buffer, 100);
		value = pt_subsCnc;
		index = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		MLE_appendObjectWithValues(&enc, dt_aId, index, &client);
		MLE_appendObjectWithValues(&enc, dt_dataId, index, &id);

		size = MLE_finalize(&enc);
		sendPacket(AM_BROADCAST_ADDR, DIST_SUBS_HEADER, buffer, size);
	}

	void setPosProxy(int32_t destination, int32_t stats, int32_t proxy) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		int32_t index = 0;

		MLE_initialize(&enc, buffer, 100);
		value = pt_dp;
		index = MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);
		index = MLE_appendObjectWithValues(&enc, dt_setup, index, NULL);
		MLE_appendObjectWithValues(&enc, dt_rssi, index, &stats);
		MLE_appendObjectWithValues(&enc, dt_loc, index, &proxy);

		size = MLE_finalize(&enc);
		sendPacket(destination, POS_ANALYSIS_CONTROL_HEADER, buffer, size);
	}

	void getPosProxy(int32_t destination) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;

		MLE_initialize(&enc, buffer, 100);
		value = pt_dq;
		MLE_appendObjectWithValues(&enc, dt_packet, 0, &value);

		size = MLE_finalize(&enc);
		sendPacket(destination, POS_ANALYSIS_CONTROL_HEADER, buffer, size);
	}

	void newSubscribeToLocation(int32_t type, int32_t id, int32_t period, int32_t duration, int32_t x, int32_t y, int32_t r) {
		ml_encoder_t enc;
		uint8_t buffer[100];
		uint8_t size = 0;
		int32_t value = 0;
		uint8_t index = 0;
		spatial_interval_t spint;

		MLE_initialize(&enc, buffer, 100);
		index = MLE_appendOV(&enc, dt_packet, pt_subs);
		value = AM_SOURCE;
		MLE_appendOSV(&enc, dt_aId, index, AM_SOURCE);
		MLE_appendObjectWithValues(&enc, dt_dataId, index, &id);

		MLE_appendObjectWithValues(&enc, dt_intTime, index, &duration);

		spint.subject = MLE_appendOV(&enc, dt_object, type);
		spint.type = dt_spiC;
		spint.c.x = x;
		spint.c.y = y;
		spint.c.z = 0;
		spint.c.r = r;
		MLE_appendSpatialInterval(&enc, &spint);

		MLE_appendOSV(&enc, dt_intTime, spint.subject, period);

		size = MLE_finalize(&enc);
		sendPacket(AM_BROADCAST_ADDR, DIST_SUBS_HEADER, buffer, size);
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
		printf("\tnewsubsarea\n");
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

	int newmltomote(ActiveMessage* pam, int argc, const char* argv[]) {
		int sleeplen = 0;
		int argi[argc];

		if(mlt.initialize("dt_types.txt") != SUCCESS) {
			if(mlt.initialize("/opt/prolib/dt_types.txt") != SUCCESS) return -1;
		}
		mlp = new MLPrint(&mlt);

		am = pam;
		am->setAddress(AM_GROUP, AM_SOURCE);
		am->registerReceiver(receive, NULL);

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
				else if(strcmp(argv[1], "newsubsarea") == 0) {
					if(processInput(argc, argv, argi, 9) == SUCCESS) newSubscribeToLocation(argi[2], argi[3], argi[4], argi[5], argi[6], argi[7], argi[8]);
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

		return 0;
	}
