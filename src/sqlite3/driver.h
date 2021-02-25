/**
 * sqlite3/driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

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

public:
	explicit SQLite3Driver(const char* filename);

	inline ~SQLite3Driver() override
	{
		sqlite3_close(this->db);
		DbDriver::~DbDriver();
	}

	[[nodiscard]]
	std::string make_select_query(
		const std::string& table_name,
		bool distinct,
		const q::condition& where_cond,
		const std::initializer_list<q::ordering>& order_by_cols,
		long int limit,
		const std::initializer_list<std::string>& group_by_cols,
		const q::condition& having_cond
	) const override;

	// In function 'handler_row(void*, void*)':
	//
	// - first parameter is initial container which is passed here as
	//   the second parameter;
	// - second parameter is row of type std::map<std::string, char*>
	//   which contains pairs (column_name, column_value).
	void run_select(
		const std::string& query, void* container, void(*handle_row)(void*, void*)
	) const override;
};

__SQLITE3_END__
