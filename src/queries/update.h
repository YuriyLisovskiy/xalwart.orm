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
	static_assert(ModelT::meta_pk_name != nullptr, "'meta_pk_name' is not initialized");

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

		for (auto attr = model.attrs_begin(); attr != model.attrs_end(); attr++)
		{
			if constexpr (ModelT::meta_omit_pk)
			{
				if (attr->first == ModelT::meta_pk_name)
				{
					continue;
				}
			}

			this->columns_data += attr->first + " = " + attr->second.get()->__repr__();
			if (std::next(attr) != model.attrs_end())
			{
				this->columns_data += ", ";
			}
		}

		std::string pk_name = get_pk_name<ModelT>();
		this->condition = q::column_condition_t(
			"", pk_name, "= " + model.__get_attr__(pk_name.c_str())->__repr__()
		);
	};

	// Sets SQL driver and prepares model's data.
	inline explicit update(abc::ISQLDriver* driver, const ModelT& model) : update(model)
	{
		this->db = driver;
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
			get_table_name<ModelT>(), this->columns_data, this->condition
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
