/**
 * sqlite3/driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./driver.h"

#ifdef USE_SQLITE3

// Core libraries.
#include <xalwart.core/string_utils.h>


__SQLITE3_BEGIN__

Driver::Driver(const char* filename)
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

void Driver::execute_query(const std::string& query) const
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

std::string Driver::run_insert(const std::string& query) const
{
	if (query.empty())
	{
		this->throw_empty_arg("query", _ERROR_DETAILS_);
	}

	char* message_error;
	std::string last_inserted_pk;
	auto extended_query = "BEGIN TRANSACTION; " + query + " SELECT last_insert_rowid(); COMMIT TRANSACTION;";
	auto ret_val = sqlite3_exec(
		this->db,
		extended_query.c_str(),
		[](void* data, int argc, char** argv, char** column_names) -> int {
			auto& last_inserted_pk = *(std::string*)data;
			last_inserted_pk = argv[0];
			return 0;
		},
		&last_inserted_pk,
		&message_error
	);

	if (ret_val != SQLITE_OK)
	{
		auto message = std::string(message_error);
		sqlite3_free(message_error);
		throw SQLError(message, _ERROR_DETAILS_);
	}

	return last_inserted_pk;
}

void Driver::run_select(
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
					auto column = str::split(column_names[i], '.', 1);
					if (!column.empty())
					{
						row[column.back()] = argv[i];
					}
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

void Driver::run_update(const std::string& query, bool batch) const
{
	this->execute_query(batch ? "BEGIN TRANSACTION; " + query + " COMMIT;" : query);
}

void Driver::run_delete(const std::string& query) const
{
	this->execute_query(query);
}

bool Driver::run_transaction(const std::function<bool()>& func) const
{
	this->execute_query("BEGIN TRANSACTION;");
	auto commit = func();
	if (commit)
	{
		this->execute_query("COMMIT TRANSACTION;");
	}
	else
	{
		this->execute_query("ROLLBACK TRANSACTION;");
	}

	return commit;
}

__SQLITE3_END__

#endif // USE_SQLITE3
