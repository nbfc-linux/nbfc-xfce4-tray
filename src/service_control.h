#ifndef NBFC_SERVICE_CONTROL_H_
#define NBFC_SERVICE_CONTROL_H_

#include "nxjson.h"

int Client_Communicate(const nx_json* in, char** buf, const nx_json** out);

#endif
