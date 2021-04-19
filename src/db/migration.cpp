/**
 * db/migration.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./migration.h"


__DB_BEGIN__

bool Migration::up(abc::ISQLSchemaEditor* editor) const
{
	if (!editor)
	{
		throw core::NullPointerException(
			"xw::orm::db::Migration: schema editor is nullptr",
			_ERROR_DETAILS_
		);
	}

	auto func = [this, editor]() -> bool
	{
		for (const auto& operation : this->operations)
		{
			operation->up(editor);
		}

		return true;
	};

	bool result;
	if (this->atomic && this->sql_driver)
	{
		result = this->sql_driver->run_transaction(func);
	}
	else
	{
		result = func();
	}

	return result;
}

bool Migration::down(abc::ISQLSchemaEditor* editor) const
{
	if (!editor)
	{
		throw core::NullPointerException(
			"xw::orm::db::Migration: schema editor is nullptr",
			_ERROR_DETAILS_
		);
	}

	auto func = [this, editor]() -> bool
	{
		for (const auto& operation : this->operations)
		{
			operation->down(editor);
		}

		return true;
	};

	bool result;
	if (this->atomic && this->sql_driver)
	{
		result = this->sql_driver->run_transaction(func);
	}
	else
	{
		result = func();
	}

	return result;
}

__DB_END__
