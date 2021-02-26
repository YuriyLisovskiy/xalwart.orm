/**
 * query/select.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: wrapper for SQL 'SELECT' statement.
 */

#pragma once

// C++ libraries.
#include <string>

// Core libraries.
#include <xalwart.core/utility.h>
#include <xalwart.core/types/string.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./operations.h"
#include "../model.h"
#include "../exceptions.h"
#include "../abc.h"


__Q_BEGIN__

template <ModelBasedType ModelT>
class select
{
protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* db = nullptr;

	// Retrieves automatically, check the default constructor.
	std::string table_name;

	// Holds pair {value, is_set}:
	//  - value: actual value which will be forwarded to query generator;
	//  - is_set: indicates if the value is set or not.
	template <typename T>
	struct pair
	{
		T value;
		bool is_set = false;

		inline void set(T v)
		{
			this->value = std::move(v);
			this->is_set = true;
		}
	};

	// Indicates whether to use distinction or not in SQL statement.
	// The default is false.
	pair<bool> distinct_;

	// Holds boolean condition for SQL 'WHERE' statement.
	pair<q::condition> where_cond_;

	// Holds columns list for SQL 'ORDER BY' statement.
	pair<std::initializer_list<q::ordering>> order_by_cols_;

	// Holds value for SQL 'LIMIT'. The default is -1.
	pair<long int> limit_;

	// Holds value for SQL 'OFFSET'. The default is -1.
	pair<long int> offset_;

	// Holds columns list for SQL 'GROUP BY' statement.
	pair<std::initializer_list<std::string>> group_by_cols_;

	// Holds boolean condition for SQL 'HAVING' statement.
	pair<q::condition> having_cond_;

public:

	// Retrieves table name of 'ModelT'. If ModelT::meta_table_name
	// is nullptr, uses 'utility::demangle(...)' method to complete
	// the operation.
	inline explicit select()
	{
		if constexpr (ModelT::meta_table_name != nullptr)
		{
			this->table_name = ModelT::meta_table_name;
		}
		else
		{
			this->table_name = utility::demangle(typeid(ModelT).name());
			this->table_name = this->table_name.substr(this->table_name.rfind(':') + 1);
		}

		this->distinct_.value = false;
		this->limit_.value = -1;
		this->offset_.value = -1;
	};

	// Sets SQL driver and calls the default constructor.
	inline explicit select(abc::ISQLDriver* driver) : select()
	{
		this->db = driver;
	};

	// Sets the distinct value.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& distinct()
	{
		if (this->distinct_.is_set)
		{
			throw QueryError(
				"'distinct' value is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->distinct_.set(true);
		return *this;
	}

	// Sets the condition for 'where' filtering.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& where(const q::condition& cond)
	{
		if (this->where_cond_.is_set)
		{
			throw QueryError(
				"'where' condition is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->where_cond_.set(cond);
		return *this;
	}

	// Sets columns for ordering.
	//
	// Throws 'QueryError' if this method is called
	// more than once with non-empty columns list.
	inline virtual select& order_by(const std::initializer_list<q::ordering>& columns)
	{
		if (this->order_by_cols_.is_set)
		{
			throw QueryError(
				"columns for ordering is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		if (columns.size())
		{
			this->order_by_cols_.set(columns);
		}

		return *this;
	}

	// Sets the limit value.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& limit(size_t limit)
	{
		if (this->limit_.is_set)
		{
			throw QueryError(
				"'limit' value is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->limit_.set(limit);
		return *this;
	}

	// Sets the offset value.
	//
	// Throws 'QueryError' if this method is called
	// more than once with positive value.
	inline virtual select& offset(size_t offset)
	{
		if (this->offset_.is_set)
		{
			throw QueryError(
				"'offset' value is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		if (offset > 0)
		{
			this->offset_.set(offset);
		}

		return *this;
	}

	// Sets columns for grouping.
	//
	// Throws 'QueryError' if this method is called
	// more than once with non-empty columns list.
	inline virtual select& group_by(const std::initializer_list<std::string>& columns)
	{
		if (this->group_by_cols_.is_set)
		{
			throw QueryError(
				"columns for grouping is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		if (columns.size())
		{
			this->group_by_cols_.set(columns);
		}

		return *this;
	}

	// Sets the condition for 'having' filtering.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& having(const q::condition& cond)
	{
		if (this->having_cond_.is_set)
		{
			throw QueryError(
				"'having' condition is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->having_cond_.set(cond);
		return *this;
	}

	// Sets SQL driver.
	inline virtual select& using_(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->db = driver;
		}

		return *this;
	}

	// Set limit value to 1, if it was not already set
	// and retrieves the first item of 'to_vector()'
	// result. In case if 'to_vector()' returns an empty
	// vector, returns null-model.
	//
	// Throws 'QueryError' when driver is not set.
	inline virtual ModelT first()
	{
		// check if `limit(...)` was not called
		if (!this->limit_.is_set)
		{
			this->limit(1);
		}

		auto values = this->to_vector();
		if (values.empty())
		{
			ModelT model;
			model.mark_as_null();
			return model;
		}

		return values[0];
	}

	// Performs an access to database. Runs SQL 'SELECT'
	// query and returns its result.
	//
	// Throws 'QueryError' when driver is not set.
	inline virtual std::vector<ModelT> to_vector() const
	{
		auto query = this->query();
		using row_t = std::map<const char*, char*>;
		using data_t = std::vector<ModelT>;
		data_t collection;
		this->db->run_select(query, &collection, [](void* container_ptr, void* row_ptr) -> void {
			auto& container = *(data_t *)container_ptr;
			auto& row = *(row_t *)row_ptr;
			ModelT model;
			for (const auto& column : row)
			{
				if (column.second)
				{
					auto len = std::strlen(column.second);
					model.__set_attr__(column.first, std::make_shared<types::String>(
						std::string(column.second, len + 1)
					));
				}
			}

			container.push_back(model);
		});

		return collection;
	}

	// Generates query using SQL driver.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline virtual std::string query() const
	{
		if (!this->db)
		{
			throw QueryError("select: database client not set", _ERROR_DETAILS_);
		}

		return this->db->make_select_query(
			this->table_name,
			this->distinct_.value,
			this->where_cond_.value,
			this->order_by_cols_.value,
			this->limit_.value,
			this->offset_.value,
			this->group_by_cols_.value,
			this->having_cond_.value
		);
	}
};

__Q_END__
