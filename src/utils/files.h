#ifndef __FILES_H__
#define __FILES_H__

const char* load_file_as_string(const char* _filename);

int save_string_to_file (const char* _str, char* _filename);

int json_handler (char* _str, char* _filename, char* _cityname);

#endif
