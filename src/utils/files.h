#ifndef __FILES_H__
#define __FILES_H__

char** loadfile(char* filename, int* len);

void save_json_to_file (const char* json, char* filename);

#endif
