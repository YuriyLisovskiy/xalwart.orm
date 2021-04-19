/**
 * sql_driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Default driver implements initialization of default SQL schema editor
 * and default SQL query builder.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./db/schema_editor.h"
#include "./sql_builder.h"


__ORM_BEGIN__

class DefaultSQLDriver : public abc::ISQLDriver
{
protected:

	// SQL Schema editor related to SQL driver.
	mutable std::shared_ptr<db::abc::ISQLSchemaEditor> sql_schema_editor;

	// SQL query builder related to SQL driver.
	mutable std::shared_ptr<abc::ISQLQueryBuilder> sql_query_builder;

public:

	// TESTME: schema_editor
	// Instantiates default schema editor if it was not
	// done yet and returns it.
	[[nodiscard]]
	inline db::abc::ISQLSchemaEditor* schema_editor() const override
	{
		if (!this->sql_schema_editor)
		{
			this->sql_schema_editor = std::make_shared<db::DefaultSQLSchemaEditor>((ISQLDriver*) this);
		}

		return this->sql_schema_editor.get();
	}

	// TESTME: query_builder
	// Instantiates default SQL query builder if it was not
	// done yet and returns it.
	[[nodiscard]]
	inline abc::ISQLQueryBuilder* query_builder() const override
	{
		if (!this->sql_query_builder)
		{
			this->sql_query_builder = std::make_shared<DefaultSQLBuilder>();
		}

		return this->sql_query_builder.get();
	}
};

__ORM_END__
