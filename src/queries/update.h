/**
 * queries/update.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: wrapper for SQL 'UPDATE' statement.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "../exceptions.h"


__Q_BEGIN__

template <ModelBasedType ModelT>
class update final
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* db = nullptr;

	// Name of the table which is retrieved from
	// `ModelT::meta_table_name` static member.
	std::string table_name;

	// Holds rows to update.
	//
	// `first`: columns with new values separated by comma.
	// Example: "name = 'Steve', age = 21".
	//
	// `second`: condition for 'WHERE' statement.
	//	Indicates what rows should be updated.
	std::vector<std::pair<std::string, q::condition_t>> rows;

protected:
	inline void append_row(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError("update: unable to update null model", _ERROR_DETAILS_);
		}

		std::string pk_name, pk_val;
		std::pair<std::string, q::condition_t> row_data;
		util::tuple_for_each(ModelT::meta_columns, [model, &pk_name, &pk_val, &row_data](auto& column)
		{
			using field_type = typename std::remove_reference<decltype(column)>::type;
			using T = typename field_type::field_type;

			if (column.is_pk)
			{
				pk_val = get_column_value_as_string<ModelT, T>(model, column);
				pk_name = column.name;

				if constexpr (ModelT::meta_omit_pk)
				{
					return true;
				}
			}

			row_data.first += column.name + " = " + get_column_value_as_string<ModelT, T>(model, column) + ", ";
			return true;
		});

		row_data.first = str::rtrim(row_data.first, ", ");
		row_data.second = q::column_condition_t(this->table_name, pk_name, "= " + pk_val);
		this->rows.push_back(row_data);
	}

public:

	// Prepares model's data.
	inline explicit update(const ModelT& model)
	{
		this->table_name = meta::get_table_name<ModelT>();
		this->append_row(model);
	};

	// Sets SQL driver.
	inline update& use(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->db = driver;
		}

		return *this;
	}

	// Generates query using SQL driver.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::string query() const
	{
		if (!this->db)
		{
			throw QueryError("update: database driver not set", _ERROR_DETAILS_);
		}

		return str::join(
			" ", this->rows.begin(), this->rows.end(),
			[this](const auto& row) -> std::string {
				return this->db->make_update_query(this->table_name, row.first, row.second);
			}
		);
	}

	// Appends model to updating list.
	//
	// Throws 'QueryError' if model is null.
	inline update& model(const ModelT& model)
	{
		this->append_row(model);
		return *this;
	}

	// Updates one row in database.
	inline void commit_one() const
	{
		if (this->rows.size() > 1)
		{
			throw QueryError(
				"update: trying to update one model, but multiple models were set",
				_ERROR_DETAILS_
			);
		}

		auto query = this->query();
		this->db->run_update(query, false);
	}

	// Updates multiple rows in database.
	inline void commit_batch() const
	{
		auto query = this->query();
		this->db->run_update(query, true);
	}
};

__Q_END__
