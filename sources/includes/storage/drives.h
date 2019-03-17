#ifndef _DRIVES_H
#define _DRIVES_H (1)
#include "../ports.h"
#include "ata.h"
#include "ahci.h"
#include "storage.h"
#include "../../std/map.h"
//extern map<char *, void * > device_names;
void drives_init();
#endif