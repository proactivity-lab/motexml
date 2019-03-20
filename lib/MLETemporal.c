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
#include "MLETemporal.h"

	LIBEXPORT uint8_t MLE_appendTemporalInterval(ml_encoder_t* enc, temporal_interval_t* interval) {
        uint8_t positionBefore = enc->currentPosition; // We try to append, but if space runs out, we can roll back
        uint8_t usedBefore = enc->usedSpace; // We try to append, but if space runs out, we can roll back
        uint8_t subject = 0;

        if((enc->buf != NULL) && (interval != NULL)) {
            switch(interval->type) {
                case dt_timeR:
                    subject = MLE_appendObjectWithValues(enc, dt_timeR, interval->subject, &(interval->elapsed));
                    if(subject > 0) {
                        if(MLE_appendObjectWithValues(enc, dt_intTime, subject, &(interval->intTime)) > 0) {
                            return subject; // Successfully added, return position of dt_timeR object
                        }
                    }
                    break;
                case dt_timeAbs:
                    subject = MLE_appendObjectWithValues(enc, dt_timeAbs, interval->subject, (int32_t*)&(interval->start)); // TODO the conversion is because UTC uses uint
                    if(subject > 0) {
                        if(MLE_appendObjectWithValues(enc, dt_intTime, subject, &(interval->intTime)) > 0) {
                            return subject; // Successfully added, return position of dt_timeAbs object
                        }
                    }
                    break;
                default:
                    return 0; // Unknown interval type
            }
            enc->currentPosition = positionBefore; // Undo changes
            enc->usedSpace = usedBefore; // Undo changes
        }
        return 0;
    }
