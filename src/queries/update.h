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

// TODO: consider bulk update.
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

	// Columns with new values separated by comma.
	// Example: "name = 'Steve', age = 21".
	std::string columns_data;

	// Condition for 'WHERE' statement.
	// Indicates what rows should be updated.
	q::condition_t condition;

public:

	// Prepares model's data.
	inline explicit update(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError("update: unable to update null model", _ERROR_DETAILS_);
		}

		this->table_name = meta::get_table_name<ModelT>();

		std::string pk_name, pk_val;
		util::tuple_for_each(ModelT::meta_columns, [this, model, &pk_name, &pk_val](auto& column)
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

			this->columns_data += column.name + " = " +
				get_column_value_as_string<ModelT, T>(model, column) + ", ";
			return true;
		});

		str::rtrim(this->columns_data, ", ");
		this->condition = q::column_condition_t(this->table_name, pk_name, "= " + pk_val);
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

		return this->db->make_update_query(
			this->table_name, this->columns_data, this->condition
		);
	}

	// Update row(s) in database.
	inline void exec()
	{
		auto query = this->query();
		this->db->run_update(query);
	}
};

__Q_END__
