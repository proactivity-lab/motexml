
# Mote "XML" Library.

The MoteXML format is a subject-object-value data encoding format for which
the human-readable representation is commonly presented using XML and a toolset
exists for easily shifting between the XML and binary representations.

This document gives an overview of protocol version 1 only!

## Data format v1

The format encodes each element using a control byte and subsequent subject,
type, value and buffer bytes. The control byte establishes the presence and
length of the other fields. Version 1 of the protocol supports only signed
integers with up to 4 bytes of length.
```
BSTTTVVV S8 T8 T8 T8 T8 V8 V8 V8 V8 BL BB .. BB
                                       ## .. ## - 1-255 buffer bytes
                                    ##----------- optional buffer length byte
                        ## ## ## ##- 0, 1, 2 or 4 value bytes
            ## ## ## ## ------------ 1, 2 or 4 type bytes
         ## ------------------------ subject byte
     +++ value (0,1,2,4 bytes)
  +++--- type  (1,2,4 bytes)
 +------ subject
+------- buffer present
```
## dt_types.txt and dt_types.h

Using the format assumes a common set of meanings for the object values. A set
of agreed object types is available in the dt_types.txt file in a
(separate repository)[https://github.com/proactivity-lab/dt-types]. The XML
tools use the dt_types.txt for input, while the header file defines all the
type names as enums and can be used from C code.

## Encoding and decoding messages

Message composition is performed using an encoder. The encoder is intitalized
with MLE_initialize and requires a buffer to work on. The encoder is passed
to any subsequent encode commands and the buffer is finalized with MLE_finalize.
Occurrance of errors is possible with MLE_errors.

### MLE_* functions
The encoder fundamentally works with an ml_object_t object, but convenience
functions exist to pass different sets of values directly.

### MLD_* functions
The decoder functions work directly on a buffer and search for matching
objects.

### MLI_* functions
The iterator functions use an interator context and are therefore more efficient
than normal decoder functions - they do not have to start from the first byte
each time.

## Library
On a Linux system the encoder/decoder functions can be used by linking against
the libmotexml library. The library is also a dependance for the Python tools
described in the Tools chapter. The library can be built with make in the lib
directory.

## Tools
Python-based transformation tools that use the library are available from
(python-motexml)[https://github.com/proactivity-lab/python-motexml]. This
includes command-line tools for encoding/decoding whole messages and a wrapper
for use in python applications.

## Embedded applications
For embedded applications it is recommended to use the enums from dt_types.h
header and simply include the mld.c, mle.c, mli.c and mlo.c files for
compilation.

## Debian package
A Debian package of the library can be made with build-deb.sh, it uses
checkinstall to package the library.
