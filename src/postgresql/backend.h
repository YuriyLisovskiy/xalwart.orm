/**
 * postgresql/backend.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Common methods for accessing the 'PostgreSQL' database.
 */

#pragma once

#ifdef USE_POSTGRESQL

// C++ libraries.
#include <string>
#include <vector>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./connection.h"
#include "../backend.h"


__ORM_POSTGRESQL_BEGIN__

// TESTME: Backend
class Backend : public DefaultSQLBackend
{
public:
	explicit Backend(size_t pool_size, const PostgreSQLCredentials& credentials);

	[[nodiscard]]
	inline std::string dbms_name() const override
	{
		return "postgresql";
	}

	// Instantiates PostgreSQL schema editor if it was not
	// done yet and returns it.
	[[nodiscard]]
	db::ISchemaEditor* schema_editor() const override;

	[[nodiscard]]
	std::vector<std::string> get_table_names() override;
};

__ORM_POSTGRESQL_END__

#endif // USE_POSTGRESQL
