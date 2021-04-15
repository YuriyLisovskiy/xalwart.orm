/**
 * driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Base driver implements generation of common SQL queries
 * which can be overwritten.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abc.h"
#include "./db/schema_editor.h"


__ORM_BEGIN__

class SQLDriverBase : public abc::ISQLDriver
{
protected:

	// Schema editor related to SQL driver.
	mutable std::shared_ptr<db::abc::ISchemaEditor> schema_editor_;

	// Helper method which throws 'QueryError' with message and
	// location for 'arg' argument name.
	void throw_empty_arg(
		const std::string& arg, int line, const char* function, const char* file
	) const;

public:

	// TESTME: schema_editor
	// Instantiates default schema editor if it was not
	// done yet and returns it.
	[[nodiscard]]
	inline db::abc::ISchemaEditor* schema_editor() const override
	{
		if (!this->schema_editor_)
		{
			this->schema_editor_ = std::make_shared<db::DefaultSchemaEditor>((ISQLDriver*) this);
		}

		return this->schema_editor_.get();
	}

	// Generates 'INSERT' query as string.
	//
	// All arguments must be non-empty, 'rows' must contain
	// at least one non-empty row.
	[[nodiscard]]
	std::string make_insert_query(
		const std::string& table_name,
		const std::string& columns,
		const std::vector<std::string>& rows
	) const override;

	// Builds 'SELECT' query to string from parts.
	//
	// !IMPORTANT!
	// This method do not prepare columns to select.
	//
	// 'table_name' must be non-empty string.
	// 'columns' must be non-empty string.
	[[nodiscard]]
	std::string compose_select_query(
		const std::string& table_name,
		const std::string& columns,
		bool distinct,
		const std::vector<q::join_t>& joins,
		const q::condition_t& where_cond,
		const std::list<q::ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::list<std::string>& group_by_cols,
		const q::condition_t& having_cond
	) const override;

	// Generates 'SELECT' query as string.
	//
	// 'table_name' must be non-empty string.
	// 'columns' must be non-empty vector.
	[[nodiscard]]
	std::string make_select_query(
		const std::string& table_name,
		const std::vector<std::string>& columns,
		bool distinct,
		const std::vector<q::join_t>& joins,
		const q::condition_t& where_cond,
		const std::list<q::ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::list<std::string>& group_by_cols,
		const q::condition_t& having_cond
	) const override;

	// Generates 'UPDATE' query as string.
	//
	// `table_name`: must be non-empty string.
	// `columns_data`: columns with data,
	// example: "column1 = data1, column2 = data2, ..."
	[[nodiscard]]
	std::string make_update_query(
		const std::string& table_name,
		const std::string& columns_data,
		const q::condition_t& condition
	) const override;

	// Generates 'DELETE' query as string.
	//
	// 'table_name' must be non-empty string.
	[[nodiscard]]
	std::string make_delete_query(
		const std::string& table_name, const q::condition_t& where_cond
	) const override;
};

__ORM_END__
