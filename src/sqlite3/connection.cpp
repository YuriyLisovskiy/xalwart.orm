/**
 * sqlite3/connection.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./connection.h"

#ifdef USE_SQLITE3


__ORM_SQLITE3_BEGIN__

SQLite3Connection::SQLite3Connection(const char* filename) : in_transaction(false)
{
	if (!filename)
	{
		this->throw_empty_arg("filename", _ERROR_DETAILS_);
	}

	::sqlite3* driver;
	if (sqlite3_open_v2(filename, &driver, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr))
//	if (sqlite3_open(filename, &driver))
	{
		throw RuntimeError(
			"error while opening sqlite3 database: " + std::string(sqlite3_errmsg(driver)),
			_ERROR_DETAILS_
		);
	}

	this->db = driver;
}

SQLite3Connection::~SQLite3Connection()
{
	if (!this->db)
	{
		if (this->in_transaction)
		{
			this->rollback_transaction();
		}

		sqlite3_close(this->db);
	}
}

void SQLite3Connection::run_query(
	const std::string& sql_query,
	const std::function<void(const std::map<std::string, char*>& /* columns */)>& row_handler,
	const std::function<void(const std::vector<char*>& /* columns */)>& vector_handler
) const
{
	try
	{
		this->run_query_unsafe(sql_query, row_handler, vector_handler);
	}
	catch (const BaseException& exc)
	{
		this->rollback_transaction();
		throw exc;
	}
	catch (const std::exception& exc)
	{
		this->rollback_transaction();
		throw exc;
	}
}

void SQLite3Connection::run_query(const std::string& sql_query, std::string& last_row_id) const
{
	auto extended_query = "BEGIN TRANSACTION; " + sql_query + " SELECT last_insert_rowid(); COMMIT TRANSACTION;";
	this->run_query(extended_query, nullptr, [&last_row_id](const std::vector<char*>& values)
	{
		last_row_id = values.front();
	});
}

void SQLite3Connection::run_query_unsafe(
	const std::string& query,
	std::function<void(const std::map<std::string, char*>&)> map_handler,
	std::function<void(const std::vector<char*>&)> vector_handler
) const
{
	if (query.empty())
	{
		this->throw_empty_arg("query", _ERROR_DETAILS_);
	}

	char* message_error;
	int ret_val = SQLITE_OK;
	if (map_handler)
	{
		using function_type = std::function<void(const std::map<std::string, char*>&)>;
		ret_val = sqlite3_exec(
			this->db,
			query.c_str(),
			[](void* data, int argc, char** argv, char** column_names) -> int {
				auto& handler = *(function_type*)data;
				std::map<std::string, char*> map;
				for (int i = 0; i < argc; i++)
				{
					// TODO: check what if column value is nullptr!
					map[column_names[i]] = argv[i];
				}

				handler(map);
				return 0;
			},
			&map_handler,
			&message_error
		);
	}
	else if (vector_handler)
	{
		using function_type = std::function<void(const std::vector<char*>&)>;
		ret_val = sqlite3_exec(
			this->db,
			query.c_str(),
			[](void* data, int argc, char** argv, char** column_names) -> int {
				auto& handler = *(function_type*)data;
				std::vector<char*> vector;
				vector.reserve(argc);
				for (int i = 0; i < argc; i++)
				{
					// TODO: check what if column value is nullptr!
					vector.emplace_back(argv[i]);
				}

				handler(vector);
				return 0;
			},
			&vector_handler,
			&message_error
		);
	}
	else
	{
		ret_val = sqlite3_exec(this->db, query.c_str(), nullptr, nullptr, &message_error);
	}

	if (ret_val != SQLITE_OK)
	{
		auto message = std::string(message_error);
		sqlite3_free(message_error);
		throw SQLError(message, _ERROR_DETAILS_);
	}
}

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
