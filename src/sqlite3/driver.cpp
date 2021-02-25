/**
 * sqlite3/driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./driver.h"

// Core libraries.
#include <xalwart.core/exceptions.h>
#include <xalwart.core/string_utils.h>


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

std::string SQLite3Driver::make_insert_query(
	const std::string& table_name,
	const std::string& columns,
	const std::vector<std::string>& rows
) const
{
	return "INSERT INTO " + table_name + " (" + columns + ") VALUES (" + str::join(
		rows.begin(), rows.end(), "), ("
	) + ");";
}

std::string SQLite3Driver::run_insert(const std::string& query, bool bulk) const
{
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
		throw core::RuntimeError(message, _ERROR_DETAILS_);
	}

	return data.second;
}

std::string SQLite3Driver::make_select_query(
	const std::string& table_name,
	bool distinct,
	const q::condition& where_cond,
	const std::initializer_list<q::ordering>& order_by_cols,
	long int limit,
	const std::initializer_list<std::string>& group_by_cols,
	const q::condition& having_cond
) const
{
	auto query = std::string("SELECT") + (distinct ? " DISTINCT" : "") + " * FROM " + table_name;

	auto where_str = (std::string)where_cond;
	if (!where_str.empty())
	{
		query += " WHERE " + where_str;
	}

	if (order_by_cols.size())
	{
		query += " ORDER BY ";
		for (auto it = order_by_cols.begin(); it != order_by_cols.end(); it++)
		{
			query += (std::string)*it;
			if (std::next(it) != order_by_cols.end())
			{
				query += ", ";
			}
		}
	}

	if (limit > -1)
	{
		query += " LIMIT " + std::to_string(limit);
	}

	if (group_by_cols.size())
	{
		query += " GROUP BY " + str::join(group_by_cols.begin(), group_by_cols.end(), ", ");
	}

	auto having_str = (std::string)having_cond;
	if (!having_str.empty())
	{
		query += " HAVING " + having_str;
	}

	return query + ";";
}

void SQLite3Driver::run_select(
	const std::string& query, void* container, void(*handle_row)(void*, void*)
) const
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
