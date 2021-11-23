/**
 * sqlite3/backend.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./backend.h"

#ifdef USE_SQLITE3

// C++ libraries.
#include <memory>

// Base libraries.
#include <xalwart.base/interfaces/orm.h>

// Orm libraries.
#include "./connection.h"
#include "./schema_editor.h"


__ORM_SQLITE3_BEGIN__

Backend::Backend(size_t pool_size, const char* filename) : DefaultSQLBackend(
	pool_size, [filename]() -> std::shared_ptr<IDatabaseConnection>
	{
		return std::make_shared<SQLite3Connection>(filename);
	}
)
{
}

db::ISchemaEditor* Backend::schema_editor() const
{
	if (!this->sql_schema_editor)
	{
		this->sql_schema_editor = std::make_shared<SchemaEditor>();
	}

	return this->sql_schema_editor.get();
}

std::vector<std::string> Backend::get_table_names(const IDatabaseConnection* connection)
{
	std::string query = "SELECT name FROM sqlite_master WHERE type ='table' AND name NOT LIKE 'sqlite_%'";
	std::shared_ptr<IDatabaseConnection> local_connection = nullptr;
	if (!connection)
	{
		local_connection = this->get_connection();
		if (!local_connection)
		{
			throw DatabaseError("Received nullptr connection", _ERROR_DETAILS_);
		}

		connection = local_connection.get();
	}

	std::vector<std::string> tables;
	connection->run_query(query, nullptr, [&tables](const std::vector<char*>& data)
	{
		tables.emplace_back(data[0]);
	});
	if (local_connection)
	{
		this->release_connection(local_connection);
	}

	return tables;
}

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
