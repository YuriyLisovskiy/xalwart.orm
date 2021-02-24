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

	// In function 'handler_row(void*, void*)':
	//
	// - first parameter is initial container which is passed here as
	//   the second parameter;
	// - second parameter is row of type std::map<std::string, char*>
	//   which contains pairs (column_name, column_value).
	void run_select(
		const std::string& query, void* container, void(*handle_row)(void*, void*)
	) override;

	// TODO: add insert, update and delete
};

__SQLITE3_END__
