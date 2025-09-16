#ifndef MISC_H
#define MISC_H

void lowercase_string(char* input);

int yes_or_no(char* question, char answer);

int line_count(const char* string);

void save_json_to_file (const char* json, char* filename);

#endif
