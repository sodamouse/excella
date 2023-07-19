#pragma once

bool create_database(const char* fp);
bool load_database(const char* fp, const char** activeDbPath);
void save_database_to_file(const char* fp);
#define reset_database() entryIdx = 0
