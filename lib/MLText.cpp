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
 * @brief Text functions.
 *
 * @author Raido Pahtma
 */
#include "MLText.h"
#include "debug.h"

error_t MLText::openFile(const char *fileName, FILE **f) {
    if(*f == NULL) {
        if((*f = fopen(fileName, "r")) != NULL) return SUCCESS;
        else pmesg(FILE_OPERATIONS_DEBUG, "Opening %s FAILED\n", fileName);
    }
    else pmesg(FILE_OPERATIONS_DEBUG, "%s Already open(descriptor not NULL)?\n", fileName);
    return FAIL;
}

error_t MLText::insertValues(FILE *f) {
	char line[160];
	int start = 0;
	int code = 0;
	int count = 0;

	while(fgets(line, sizeof(line), f) != NULL) { // Read lines until EOF - fgets returns NULL
		count++;
		switch(line[0]) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				for(int i=0; i<80; i++) {
					if(line[i] == ',') {
						line[i] = '\0';
						sscanf(line, "%x", &code);
						start = i + 1;
						for(int j=start; j<start+CODE_STR_LENGTH; j++) {
							if(line[j] == ',' || line[j] == '\n' || line[j] == ' ' || line[j] == '#') {
								line[j] = '\0';
								strcpy(dtcodes[code], &line[start]);
								break;
							}
						}
						break;
					}
					else if(line[i] == '\n') {
						line[i] = '\0';
						code = atoi(&line[start]);
						printf("Error, no string assigned to code %x!\n", code);
					}
				}
				break;
				return SUCCESS;
			case '#': // Comment
				break;
			case ' ':
			case '\n':
				break;
			default:
				printf("Bad input line: %i\n", count);
				break;
		}
	}
	return SUCCESS;
}

error_t MLText::initialize(const char* path) {
	error_t error = FAIL;
	FILE *f = NULL;
	if(openFile(path, &f) == SUCCESS) {
		error = insertValues(f);
		fclose(f);
	}
	return error;
}

void MLText::printAllValues() {
	for(int i=0;i<256;i++) printf("0x%x, %s\n", i, dtcodes[i]);
}

char* MLText::toString(int32_t code) {
	if((code > 0) && (code < 255)) return dtcodes[code];
	return NULL;
}

MLText::MLText() {
	for(int i=0;i<256;i++) sprintf(dtcodes[i], "0x%x", i);
}

MLText::~MLText() {
	// TODO Auto-generated destructor stub
}
