/**
 * db/abc.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Abstract base classes for `db` module.
 */

#pragma once

// C++ libraries.
#include <list>

// Module definitions.
#include "./_def_.h"


__DB_BEGIN__

enum sql_column_type
{
	BOOL_T,
	CHAR_T, VARCHAR_T, TEXT_T,
	SMALLINT_T, INT_T, BIGINT_T,
	SMALL_SERIAL_T, SERIAL_T, BIG_SERIAL_T,
	REAL_T, DOUBLE_T
};

enum on_action
{
	SET_NULL, SET_DEFAULT, RESTRICT, NO_ACTION, CASCADE
};

__DB_END__


__DB_ABC_BEGIN__

class ISQLSchemaEditor
{
public:
	// SQL builders.
	[[nodiscard]]
	virtual std::string sql_data_column(
		sql_column_type type, const std::string& name,
		bool null, bool primary_key, bool unique, const std::string& check
	) const = 0;

	[[nodiscard]]
	virtual std::string sql_text_column(
		sql_column_type type, const std::string& name, long int max_len,
		bool null, bool primary_key, bool unique, const std::string& check
	) const = 0;

	[[nodiscard]]
	virtual std::string sql_foreign_key(
		const std::string& name,
		const std::string& parent, const std::string& parent_key,
		on_action on_delete, on_action on_update
	) const = 0;

	// SQL executors.
	virtual void create_table(
		const std::string& name,
		const std::list<std::string>& columns,
		const std::list<std::string>& constraints
	) const = 0;

	virtual void drop_table(const std::string& name) const = 0;
};

class IOperation
{
public:
	virtual void up(ISQLSchemaEditor* editor) const = 0;

	virtual void down(ISQLSchemaEditor* editor) const = 0;
};

__DB_ABC_END__
