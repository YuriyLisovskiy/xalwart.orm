/**
 * backend.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./backend.h"

// Orm libraries.
#include "./db/schema_editor.h"
#include "./sql_builder.h"


__ORM_BEGIN__

void DefaultSQLBackend::create_pool()
{
	std::lock_guard<std::mutex> locker(this->_mutex);
	for (auto i = 0; i < this->_pool_size; i++)
	{
		this->_connection_pool.emplace(std::move(this->_connection_builder()));
	}
}

std::shared_ptr<IDatabaseConnection> DefaultSQLBackend::get_connection()
{
	std::unique_lock<std::mutex> lock(this->_mutex);
	while (this->_connection_pool.empty())
	{
		this->_condition.wait(lock);
	}

	auto connection = this->_connection_pool.front();
	this->_connection_pool.pop();
	return connection;
}

void DefaultSQLBackend::release_connection(const std::shared_ptr<IDatabaseConnection>& connection)
{
	std::unique_lock<std::mutex> lock(this->_mutex);
	this->_connection_pool.push(connection);
	lock.unlock();
	this->_condition.notify_one();
}

ISQLQueryBuilder* DefaultSQLBackend::sql_builder() const
{
	if (!this->sql_query_builder)
	{
		this->sql_query_builder = std::make_shared<DefaultSQLBuilder>();
	}

	return this->sql_query_builder.get();
}

db::ISchemaEditor* DefaultSQLBackend::schema_editor() const
{
	if (!this->sql_schema_editor)
	{
		this->sql_schema_editor = std::make_shared<db::DefaultSQLSchemaEditor>();
	}

	return this->sql_schema_editor.get();
}

__ORM_END__
