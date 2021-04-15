/**
 * db/migration.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./migration.h"


__DB_BEGIN__

bool Migration::up(abc::ISchemaEditor* schema_editor) const
{
	if (!schema_editor)
	{
		throw core::NullPointerException(
			"xw::orm::db::Migration: schema editor is nullptr",
			_ERROR_DETAILS_
		);
	}

	auto func = [this, schema_editor]() -> bool
	{
		for (const auto& operation : this->operations)
		{
			operation->up(schema_editor);
		}

		return true;
	};

	bool result;
	if (this->atomic && this->db)
	{
		result = this->db->run_transaction(func);
	}
	else
	{
		result = func();
	}

	return result;
}

bool Migration::down(abc::ISchemaEditor* schema_editor) const
{
	if (!schema_editor)
	{
		throw core::NullPointerException(
			"xw::orm::db::Migration: schema editor is nullptr",
			_ERROR_DETAILS_
		);
	}

	auto func = [this, schema_editor]() -> bool
	{
		for (const auto& operation : this->operations)
		{
			operation->down(schema_editor);
		}

		return true;
	};

	bool result;
	if (this->atomic && this->db)
	{
		result = this->db->run_transaction(func);
	}
	else
	{
		result = func();
	}

	return result;
}

__DB_END__
