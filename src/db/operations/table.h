/**
 * db/operations/table.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// Core libraries.
#include <xalwart.core/exceptions.h>

// Orm libraries.
#include "../abc.h"


__DB_BEGIN__

// TESTME: TableOperation
// Base class for migration operations.
class TableOperation : public abc::IOperation
{
protected:
	std::string table_name;

public:
	TableOperation() = default;

	inline explicit TableOperation(std::string name) : table_name(std::move(name))
	{
	}

	[[nodiscard]]
	inline const std::string& name() const
	{
		return this->table_name;
	}
};

// TESTME: CreateTableOperation
// Create a model's table.
class CreateTableOperation : public TableOperation
{
protected:
	std::list<std::string> columns;
	std::list<std::string> constraints;

public:
	inline explicit CreateTableOperation(
		const std::string& name,
		std::list<std::string> columns,
		std::list<std::string> constraints
	) : TableOperation(name), columns(std::move(columns)), constraints(std::move(constraints))
	{
	}

	void up(abc::ISchemaEditor* schema_editor) const override
	{
		if (!schema_editor)
		{
			throw core::NullPointerException(
				"xw::orm::db::CreateModel: schema editor is nullptr",
				_ERROR_DETAILS_
			);
		}

		schema_editor->create_table(this->table_name, this->columns, this->constraints);
	}

	void down(abc::ISchemaEditor* schema_editor) const override
	{
		if (!schema_editor)
		{
			throw core::NullPointerException(
				"xw::orm::db::CreateModel: schema editor is nullptr",
				_ERROR_DETAILS_
			);
		}

		schema_editor->drop_table(this->table_name);
	}
};

__DB_END__
