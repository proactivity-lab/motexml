#include "MLE.h"
#include "MLD.h"
#include "MLI.h"
#include "MLESpatial.h"
#include "MLDSpatial.h"
#include "MLETemporal.h"
#include "MLDTemporal.h"
#include "MLObjects.h"
#include "MLCompare.h"
module MLFormatP {
	uses {
		interface LocalTime<TMilli>;
	}
}
implementation {

	uint32_t MLDTemporal_timeNow() @C() {
		return call LocalTime.get();
	}

	#include "MLE.c"
	#include "MLD.c"
	#include "MLI.c"
	#include "MLESpatial.c"
	#include "MLDSpatial.c"
	#include "MLETemporal.c"
	#include "MLDTemporal.c"
	#include "MLObjects.c"
	#include "MLCompare.c"

}
