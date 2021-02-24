/**
 * sqlite3/driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./driver.h"

// C++ libraries.
// TODO

// Core libraries.
#include <xalwart.core/exceptions.h>

// Orm libraries.
// TODO

__SQLITE3_BEGIN__

SQLite3Driver::SQLite3Driver(const char* filename)
{
	::sqlite3* driver;
	if (sqlite3_open(filename, &driver))
	{
		throw core::RuntimeError(
			"Error while opening sqlite3 database: " + std::string(sqlite3_errmsg(driver)),
			_ERROR_DETAILS_
		);
	}

	this->db = driver;
}

void SQLite3Driver::run_select(
	const std::string& query, void* container, void(*handle_row)(void*, void*)
)
{
	char* message_error;
	std::pair<void*, void(*)(void*, void*)> data(container, handle_row);
	auto ret_val = sqlite3_exec(
		this->db,
		query.c_str(),
		[](void* data, int argc, char** argv, char** column_names) -> int {
			std::map<const char*, char*> row;
			for (int i = 0; i < argc; i++)
			{
				row[column_names[i]] = argv[i];
			}

			auto& pair = *(std::pair<void*, void(*)(void*, void*)>*)data;
			pair.second(pair.first, &row);
			return 0;
		},
		&data,
		&message_error
	);

	if (ret_val != SQLITE_OK)
	{
		auto message = std::string(message_error);
		sqlite3_free(message_error);
		throw core::RuntimeError(message, _ERROR_DETAILS_);
	}
}

__SQLITE3_END__
