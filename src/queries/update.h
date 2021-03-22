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

// TESTME: update
template <ModelBasedType ModelT>
class update
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* db = nullptr;

	std::string columns_data;

	q::condition_t condition;

public:

	// Prepares model's data.
	inline explicit update(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError("update: unable to update null model", _ERROR_DETAILS_);
		}

		std::string pk_name, pk_val;
		util::tuple_for_each(ModelT::meta_columns, [this, model, &pk_name, &pk_val](auto& column)
		{
			using field_type = typename std::remove_reference<decltype(column)>::type;
			using T = typename field_type::field_type;

			if (column.is_pk)
			{
				if constexpr (std::is_fundamental_v<T>)
				{
					pk_val = std::to_string(model.*column.member_pointer);
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					pk_val = "'" + model.*column.member_pointer + "'";
				}
				else
				{
					throw QueryError(
						"update: column types other than std::string and fundamentals are not supported",
						_ERROR_DETAILS_
					);
				}

				pk_name = column.name;

				if constexpr (ModelT::meta_omit_pk)
				{
					return true;
				}
			}

			this->columns_data += column.name + " = ";
			if constexpr (std::is_fundamental_v<T>)
			{
				this->columns_data += std::to_string(model.*column.member_pointer);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				this->columns_data += "'" + model.*column.member_pointer + "'";
			}
			else
			{
				throw QueryError(
					"update: column types other than std::string and fundamentals are not supported",
					_ERROR_DETAILS_
				);
			}

			this->columns_data += ", ";
			return true;
		});

		str::rtrim(this->columns_data, ", ");
		this->condition = q::column_condition_t("", pk_name, "= " + pk_val);
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
			util::get_table_name<ModelT>(), this->columns_data, this->condition
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
