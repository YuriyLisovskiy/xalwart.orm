/**
 * sqlite3/driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

#ifdef USE_SQLITE3

// C++ libraries.
// TODO

// SQLite
#include <sqlite3.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../driver.h"

__SQLITE3_BEGIN__

class SQLite3Driver : public DbDriver
{
protected:
	::sqlite3* db = nullptr;

protected:
	void throw_empty_arg(
		const std::string& arg, int line, const char* function, const char* file
	) const;

public:
	explicit SQLite3Driver(const char* filename);

	inline ~SQLite3Driver() override
	{
		sqlite3_close(this->db);
		DbDriver::~DbDriver();
	}

	// insert row(s)
	[[nodiscard]]
	std::string make_insert_query(
		const std::string& table_name,
		const std::string& columns,
		const std::vector<std::string>& rows
	) const override;

	[[nodiscard]]
	std::string run_insert(const std::string& query, bool bulk) const override;

	// select rows
	[[nodiscard]]
	std::string make_select_query(
		const std::string& table_name,
		bool distinct,
		const q::condition& where_cond,
		const std::initializer_list<q::ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::initializer_list<std::string>& group_by_cols,
		const q::condition& having_cond
	) const override;

	void run_select(
		const std::string& query,
		void* container,
		void(*handle_row)(void* container, void* row_map)
	) const override;
};

__SQLITE3_END__

#endif // USE_SQLITE3
