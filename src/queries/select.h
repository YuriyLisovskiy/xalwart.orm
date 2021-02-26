/**
 * query/select.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
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
	abc::ISQLDriver* db = nullptr;

	std::string table_name;

	bool distinct_ = false;
	q::condition where_cond_;
	std::initializer_list<q::ordering> order_by_cols_;
	long int limit_ = -1;
	long int offset_ = -1;
	std::initializer_list<std::string> group_by_cols_;
	q::condition having_cond_;

	uint8_t masks[5] = {
		0b00000001, // disables method 'where'
		0b00000011, // disables method 'order_by' and all above
		0b00000111, // disables method 'limit' and all above
		0b00001111, // disables method 'group_by' and all above
		0b00011111  // disables method 'having' and all above
	};

	// Holds value to indicate what methods is disabled.
	uint8_t disabled = 0x00;

protected:
	[[nodiscard]]
	inline virtual bool is_disabled(uint item) const
	{
		return (this->disabled >> item) & 1UL;
	}

public:
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
	};

	inline explicit select(abc::ISQLDriver* driver) : select()
	{
		this->db = driver;
	};

	inline select& distinct(bool distinct=true)
	{
		this->distinct_ = distinct;
		return *this;
	}

	inline virtual select& where(const q::condition& cond)
	{
		if (this->is_disabled(0))
		{
			throw QueryError(
				"unable to set 'WHERE' condition, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->where_cond_ = cond;
		this->disabled |= this->masks[0];
		return *this;
	}

	inline virtual select& order_by(const std::initializer_list<q::ordering>& columns)
	{
		if (this->is_disabled(1))
		{
			throw QueryError(
				"unable to set 'ORDER BY' columns, check method call sequence", _ERROR_DETAILS_
			);
		}

		if (columns.size())
		{
			this->order_by_cols_ = columns;
			this->disabled |= this->masks[1];
		}

		return *this;
	}

	inline virtual select& limit(size_t limit, size_t offset)
	{
		if (this->is_disabled(2))
		{
			throw QueryError(
				"unable to set 'LIMIT' value, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->limit_ = limit;
		if (offset > 0)
		{
			this->offset_ = offset;
		}

		this->disabled |= this->masks[2];
		return *this;
	}

	inline select& limit(size_t limit)
	{
		return this->limit(limit, 0);
	}

	inline virtual select& group_by(const std::initializer_list<std::string>& columns)
	{
		if (this->is_disabled(3))
		{
			throw QueryError(
				"unable to set 'GROUP BY' columns, check method call sequence", _ERROR_DETAILS_
			);
		}

		if (columns.size())
		{
			this->group_by_cols_ = columns;
			this->disabled |= this->masks[3];
		}

		return *this;
	}

	inline virtual select& having(const q::condition& cond)
	{
		if (this->is_disabled(4))
		{
			throw QueryError(
				"unable to set 'HAVING' condition, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->having_cond_ = cond;
		this->disabled |= this->masks[4];
		return *this;
	}

	inline virtual select& using_(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->db = driver;
		}

		return *this;
	}

	inline virtual ModelT first()
	{
		// check if `limit(...)` was not called
		if (!this->is_disabled(2))
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

	[[nodiscard]]
	inline virtual std::string query() const
	{
		if (!this->db)
		{
			throw QueryError("select: database client not set", _ERROR_DETAILS_);
		}

		return this->db->make_select_query(
			this->table_name,
			this->distinct_,
			this->where_cond_,
			this->order_by_cols_,
			this->limit_,
			this->offset_,
			this->group_by_cols_,
			this->having_cond_
		);
	}
};

__Q_END__
