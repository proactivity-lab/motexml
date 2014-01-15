#
# Copyright (c) 2011 Tallinn University of Technology
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the
#   distribution.
# - Neither the name of the copyright holders nor the names of
#   its contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#
# @author Raido Pahtma
#
import sys
import time

from monkeytos.serial_wrapper import SerialWrapper
from monkeytos.monkeytos import MonkeyTos

import logging_setup.logging_setup as logging_setup
logging_setup.setup("xmlsniffy", "log")

import logging
log = logging.getLogger(__name__)

import motexml.motexml as motexml

try:
    PORT = sys.argv[1]
except:
    PORT = "/dev/ttyUSB0"

try:
    BAUD = sys.argv[2]
except:
    BAUD = 115200

try:
    trans = motexml.MoteXMLTranslator("dt_types.txt")
    g_am = MonkeyTos(SerialWrapper(PORT, BAUD))

    xml_am_list = (110, 111)

    while 1:
        packet = g_am.receive()
        if packet:
            source, destination, am, payload = packet
            if am in xml_am_list:
                try:
                    xdata = trans.translate_to_xml(payload)
                    if xdata is not None:
                        log.info("AM[%03i]%04x->%04x ---------------------------------------\n%s" % (am, source, destination, trans.tostring(xdata)))
                except:
                    log.exception("")
                    log.info("AM[%03i]%04x->%04x err %s" % (source, destination, am, payload.encode("hex")))
            else:
                log.info("AM[%03i]%04x->%04x %s" % (am, source, destination, payload.encode("hex")))

        time.sleep(0.01)

except:
    log.exception("")
    sys.exit(1)
