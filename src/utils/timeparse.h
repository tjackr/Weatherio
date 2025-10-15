#ifndef __TIMEPARSE_H__
#define __TIMEPARSE_H__

#include <time.h>


time_t parse_iso_datetime(const char* _time_str);

const char* epoch_to_iso_string(time_t* _epoch);

#endif
