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
#include <xalwart.base/string_utils.h>
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
		this->sql_schema_editor = std::make_shared<SchemaEditor>((IBackend*)this);
	}

	return this->sql_schema_editor.get();
}

std::vector<std::string> Backend::get_table_names()
{
	std::string query = "SELECT name FROM sqlite_master WHERE type ='table' AND name NOT LIKE 'sqlite_%'";
	auto connection = this->get_connection();
	if (!connection)
	{
		throw SQLError("Received nullptr connection", _ERROR_DETAILS_);
	}

	std::vector<std::string> tables;
	connection->run_query(query, nullptr, [&tables](const std::vector<char*>& data)
	{
		tables.emplace_back(data[0]);
	});
	this->release_connection(connection);
	return tables;
}

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
