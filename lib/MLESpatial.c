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
 * @brief Encode functions.
 *
 * @author Raido Pahtma
 */
#include "MLESpatial.h"

    LIBEXPORT uint8_t MLE_appendSpatialInterval(ml_encoder_t* enc, spatial_interval_t* interval) {
        uint8_t positionBefore = enc->currentPosition; // We try to append, but if space runs out, we can roll back
        uint8_t usedBefore = enc->usedSpace; // We try to append, but if space runs out, we can roll back
        uint8_t subject = 0; // Position of the interval object
        bool pointsWereAdded = TRUE; // Used for dt_spiA
        int32_t pointValue = 0; // Used for dt_spiA, index of the point in the current area definition
        int8_t i = 0;

        if((enc->buf != NULL) && (interval != NULL)) {
            switch(interval->type) {
//				case dt_spiA:
//					subject = MLE_appendObjectWithValues(enc, dt_spiA, interval->subject, NULL);
//					if(subject > 0) {
//						// Add 4 points
//						for(i=0;i<4;i++) {
//							pointValue = i + 1;
//							if(MLE_appendPoint2D(enc, interval->a.x[i], interval->a.y[i], subject, &pointValue) <= 0) { // Failed to add, must roll back
//								pointsWereAdded = FALSE;
//								break;
//							}
//						}
//						if(pointsWereAdded) {
//							return subject; // Successfully added, return position of dt_spiA object
//						}
//					}
//                  break;
                case dt_spiC:
                    subject = MLE_appendObjectWithValues(enc, dt_spiC, interval->subject, interval->c.r == 0 ? NULL : &(interval->c.r));
                    if(subject > 0) {
                        if(MLE_appendObjectWithValues(enc, dt_locX, subject, &(interval->c.x)) > 0) {
                            if(MLE_appendObjectWithValues(enc, dt_locY, subject, &(interval->c.y)) > 0) {
                                if(MLE_appendObjectWithValues(enc, dt_locZ, subject, &(interval->c.z)) > 0) {
                                    return subject; // Successfully added, return position of dt_spiC object
                                }
                            }
                        }
                    }
                    break;
                case dt_spiV:
                    subject = MLE_appendObjectWithValues(enc, dt_spiV, interval->subject, NULL);
                    if(subject > 0) {
                        // Add 2 points
                        for(i=0;i<2;i++) {
                            pointValue = i + 1;
                            if(MLE_appendPoint2D(enc, interval->v.x[i], interval->v.y[i], subject, &pointValue) <= 0) { // Failed to add, must roll back
                                pointsWereAdded = FALSE;
                                break;
                            }
                        }
                        if(pointsWereAdded) {
                            return subject; // Successfully added, return position of dt_spiA object
                        }
                    }
                    break;
//                case dt_spiD:
//                	subject = MLE_appendObjectWithValues(enc, dt_spiD, interval->subject, interval->d.id == 0 ? NULL : &(interval->d.id));
//                	if(subject > 0) {
//                		return subject; // Successfully added, return position of dt_spiD object
//                	}
//                	break;
                default:
                    return 0; // Unknown interval type
            }
            enc->currentPosition = positionBefore; // Undo changes
            enc->usedSpace = usedBefore; // Undo changes
        }
        return 0;
    }

    LIBEXPORT uint8_t MLE_appendPoint2D(ml_encoder_t* enc, int32_t x, int32_t y, uint8_t subject, int32_t* value) {
        uint8_t positionBefore = enc->currentPosition; // We try to append, but if space runs out, we can roll back
        uint8_t usedBefore = enc->usedSpace; // We try to append, but if space runs out, we can roll back
        uint8_t position = 0; // Position of the interval object

        if(enc->buf != NULL) {
            position = MLE_appendObjectWithValues(enc, dt_location, subject, value);
            if(position > 0) {
                if(MLE_appendObjectWithValues(enc, dt_locX, position, &x) > 0) {
                    if(MLE_appendObjectWithValues(enc, dt_locY, position, &y) > 0) {
                        return position; // Successfully added, return position of dt_loc object
                    }
                }
                enc->currentPosition = positionBefore; // Undo changes
                enc->usedSpace = usedBefore; // Undo changes
            }
        }
        return 0;
    }
