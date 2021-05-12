/**
 * db/abc.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Abstract base classes for `db` module.
 */

#pragma once

// Core libraries.
#include <xalwart.core/datetime.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./state.h"


__ORM_DB_ABC_BEGIN__

class ISchemaEditor
{
protected:
	[[nodiscard]]
	virtual std::string quote_name(const std::string& s) const = 0;

public:
//	[[nodiscard]]
//	virtual std::string sql_type_to_string(sql_column_type type) const = 0;
//
//	// SQL builders.
//	[[nodiscard]]
//	virtual std::string sql_column(
//		sql_column_type type, const std::string& name,
//		const std::optional<size_t>& max_len,
//		const std::optional<bool>& null,
//		bool primary_key,
//		bool unique,
//		bool autoincrement,
//		const std::string& check,
//		const std::string& default_
//	) const = 0;
//
//	[[nodiscard]]
//	virtual std::string sql_foreign_key(
//		const std::string& name,
//		const std::string& parent, const std::string& parent_key,
//		on_action on_delete, on_action on_update
//	) const = 0;

	// SQL executors.
	virtual void create_table(const table_state& table) const = 0;

	virtual void drop_table(const std::string& name) const = 0;

	virtual void create_column(
		const std::string& table_name, const column_state& column
	) const = 0;

	virtual void drop_column(
		const table_state& table, const column_state& column
	) const = 0;
};

class IOperation
{
public:
	virtual void update_state(project_state& st) const = 0;

	virtual void forward(
		const ISchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const = 0;

	virtual void backward(
		const ISchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const = 0;
};

__ORM_DB_ABC_END__
