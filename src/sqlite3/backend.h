/**
 * sqlite3/backend.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Common methods for accessing the 'sqlite3' database.
 */

#pragma once

#ifdef USE_SQLITE3

// C++ libraries.
#include <string>
#include <vector>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../backend.h"


__ORM_SQLITE3_BEGIN__

// TESTME: Backend
class Backend : public DefaultSQLBackend
{
public:
	explicit Backend(size_t pool_size, const char* filename);

	[[nodiscard]]
	inline std::string dbms_name() const override
	{
		return "sqlite";
	}

	// Instantiates SQLite3 schema editor if it was not
	// done yet and returns it.
	[[nodiscard]]
	db::ISchemaEditor* schema_editor() const override;

	[[nodiscard]]
	std::vector<std::string> get_table_names(const IDatabaseConnection* connection) override;
};

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
