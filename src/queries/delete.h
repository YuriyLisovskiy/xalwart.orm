/**
 * queries/delete.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Wrapper for SQL 'DELETE' statement.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "../exceptions.h"
#include "./conditions.h"


__ORM_Q_BEGIN__

template <db::model_based_type ModelT>
class delete_ final
{
	static_assert(ModelT::meta_table_name != nullptr, "xw::orm::q::delete: 'meta_table_name' is not initialized");

protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* sql_driver = nullptr;

	// Holds condition for SQL 'WHERE' statement.
	QValue<Condition> where_cond;

	// List of primary keys to delete. It will be used by
	// default if `where` is not called.
	std::vector<std::string> pks{};

protected:
	inline void append_row(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError("xw::orm::q::delete: unable to delete null model", _ERROR_DETAILS_);
		}

		util::tuple_for_each(ModelT::meta_columns, [this, model](auto& column)
		{
			if (column.is_pk)
			{
				this->pks.push_back(column.as_string(model));
				return false;
			}

			return true;
		});
	}

public:

	// Appends model's pk to deletion list.
	explicit delete_(const ModelT& model)
	{
		this->append_row(model);
	};

	// Sets SQL driver.
	virtual delete_& use(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->sql_driver = driver;
		}

		return *this;
	}

	// Generates query using SQL driver.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::string query() const
	{
		if (!this->sql_driver)
		{
			throw QueryError("xw::orm::q::delete: database driver not set", _ERROR_DETAILS_);
		}

		auto condition = this->where_cond;
		if (!condition.is_set)
		{
			util::tuple_for_each(ModelT::meta_columns, [this, &condition](auto& column)
			{
				if (column.is_pk)
				{
					condition.set(ColumnCondition(
						db::get_table_name<ModelT>(),
						column.name,
						"IN (" + str::join(", ", this->pks.begin(), this->pks.end()) + ")"
					));
					return false;
				}

				return true;
			});
		}

		auto sql_builder = this->sql_driver->query_builder();
		if (!sql_builder)
		{
			throw QueryError("xw::orm::q::delete: SQL builder is not initialized", _ERROR_DETAILS_);
		}

		return sql_builder->sql_delete(db::get_table_name<ModelT>(), condition.value);
	}

	// Appends model's pk to deletion list.
	inline delete_& model(const ModelT& model)
	{
		this->append_row(model);
		return *this;
	}

	// Sets the condition for 'where' filtering.
	inline delete_& where(const Condition& cond)
	{
		if (this->where_cond.is_set)
		{
			this->where_cond.set(this->where_cond.value & cond);
		}
		else
		{
			this->where_cond.set(cond);
		}

		return *this;
	}

	// Performs the deletion.
	// If no models were set, executes `DELETE` without
	// condition, otherwise generates it from primary keys
	// if it was not set manually.
	inline void commit() const
	{
		auto query = this->query();
		this->sql_driver->run_delete(query);
	}
};

__ORM_Q_END__
