/**
 * sqlite3/driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#ifdef USE_SQLITE3

#include "./driver.h"

// Core libraries.
#include <xalwart.core/string_utils.h>


__SQLITE3_BEGIN__

SQLite3Driver::SQLite3Driver(const char* filename)
{
	if (!filename)
	{
		this->throw_empty_arg("filename", _ERROR_DETAILS_);
	}

	::sqlite3* driver;
	if (sqlite3_open(filename, &driver))
	{
		throw core::RuntimeError(
			"error while opening sqlite3 database: " + std::string(sqlite3_errmsg(driver)),
			_ERROR_DETAILS_
		);
	}

	this->db = driver;
}

std::string SQLite3Driver::run_insert(const std::string& query, bool bulk) const
{
	if (query.empty())
	{
		this->throw_empty_arg("query", _ERROR_DETAILS_);
	}

	char* message_error;
	using data_t = std::pair<bool, std::string>;
	data_t data(bulk, "");
	auto extended_query = bulk ? query : (
		"BEGIN TRANSACTION; " + query + " SELECT last_insert_rowid(); COMMIT;"
	);
	auto ret_val = sqlite3_exec(
		this->db,
		extended_query.c_str(),
		[](void* data, int argc, char** argv, char** column_names) -> int {
			auto& pair = *(data_t*)data;
			if (!pair.first)
			{
				pair.second = argv[0];
			}

			return 0;
		},
		&data,
		&message_error
	);

	if (ret_val != SQLITE_OK)
	{
		auto message = std::string(message_error);
		sqlite3_free(message_error);
		throw SQLError(message, _ERROR_DETAILS_);
	}

	return data.second;
}

void SQLite3Driver::run_select(
	const std::string& query, void* container, void(*handle_row)(void*, void*)
) const
{
	if (query.empty())
	{
		this->throw_empty_arg("query", _ERROR_DETAILS_);
	}

	char* message_error;
	std::pair<void*, void(*)(void*, void*)> data(container, handle_row);
	auto ret_val = sqlite3_exec(
		this->db,
		query.c_str(),
		[](void* data, int argc, char** argv, char** column_names) -> int {
			auto& pair = *(std::pair<void*, void(*)(void*, void*)>*)data;
			if (pair.second)
			{
				std::map<std::string, char*> row;
				for (int i = 0; i < argc; i++)
				{
					auto column = str::lsplit_one(column_names[i], '.');
					if (column.second.empty())
					{
						column.second = column.first;
					}

					row[column.second] = argv[i];
				}

				pair.second(pair.first, &row);
			}

			return 0;
		},
		&data,
		&message_error
	);

	if (ret_val != SQLITE_OK)
	{
		auto message = std::string(message_error);
		sqlite3_free(message_error);
		throw SQLError(message, _ERROR_DETAILS_);
	}
}

void SQLite3Driver::run_update(const std::string& query) const
{
	// TODO: run_update(const std::string& query)
}

void SQLite3Driver::run_delete(const std::string& query) const
{
	char* message_error;
	auto exit = sqlite3_exec(this->db, query.c_str(), nullptr, nullptr, &message_error);
	if (exit != SQLITE_OK)
	{
		auto message = std::string(message_error);
		sqlite3_free(message_error);
		throw SQLError(message, _ERROR_DETAILS_);
	}
}

__SQLITE3_END__

#endif // USE_SQLITE3
