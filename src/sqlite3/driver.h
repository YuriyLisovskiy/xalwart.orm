/**
 * sqlite3/driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: provides common methods for accessing the 'sqlite3' database.
 */

#pragma once

#ifdef USE_SQLITE3

// SQLite
#include <sqlite3.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../driver.h"


__SQLITE3_BEGIN__

class SQLite3Driver : public SQLDriverBase
{
protected:
	::sqlite3* db = nullptr;

public:
	explicit SQLite3Driver(const char* filename);

	inline ~SQLite3Driver() override
	{
		sqlite3_close(this->db);
		SQLDriverBase::~SQLDriverBase();
	}

	[[nodiscard]]
	inline std::string name() const override
	{
		return "sqlite3";
	}

	// insert row(s)
	[[nodiscard]]
	std::string run_insert(const std::string& query, bool bulk) const override;

	// select rows
	void run_select(
		const std::string& query,
		void* container,
		void(*handle_row)(void* container, void* row_map)
	) const override;

	// update row(s)
	// TODO: update row(s)

	// delete row(s)
	void run_delete(const std::string& query) const override;
};

__SQLITE3_END__

#endif // USE_SQLITE3
