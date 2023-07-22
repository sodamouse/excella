#pragma once

bool create_database(const char* fp);
bool load_database(const char* fp);
void save_database_to_file(const char* fp);
void reset_database();
