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

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./operations.h"
#include "../model.h"
#include "../exceptions.h"
#include "../driver.h"


__Q_BEGIN__

template <typename T, typename = int>
struct has_meta_table_name : std::false_type { };

template <typename T>
struct has_meta_table_name <T, decltype((void) T::Meta::table_name, 0)> : std::true_type { };

template <ModelBasedType ModelT>
class select
{
protected:
	DbDriver* db = nullptr;

	bool prepared = false;
	std::string query;
	std::string table_name;

	bool distinct = false;

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
	inline virtual void prepare_query()
	{
		if (!this->prepared)
		{
			this->query = std::string("SELECT") + (
				this->distinct ? " DISTINCT" : ""
			) + " * FROM " + this->table_name + this->query + ';';
			this->prepared = true;
		}
	}

	inline virtual std::string join_list(const std::initializer_list<std::string>& columns)
	{
		std::string result;
		for (auto it = columns.begin(); it != columns.end(); it++)
		{
			auto item = *it;
			if (!item.empty())
			{
				result += item;
				if (std::next(it) != columns.end())
				{
					result += ", ";
				}
			}
		}

		return result;
	}

	inline virtual bool is_disabled(uint item)
	{
		return (this->disabled >> item) & 1UL;
	}

public:
	inline explicit select()
	{
		if constexpr (has_meta_table_name<ModelT>::value)
		{
			this->table_name = ModelT::Meta::table_name;
		}
		else
		{
			this->table_name = utility::demangle(typeid(ModelT).name());
			this->table_name = this->table_name.substr(this->table_name.rfind(':') + 1);
		}
	};

	inline explicit select(DbDriver* db) : select()
	{
		this->db = db;
	};

	inline virtual select& where(const q::condition& op)
	{
		if (this->is_disabled(0))
		{
			throw QueryError(
				"unable to set 'WHERE' condition, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->query += " WHERE " + (std::string)op;
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
			this->query += " ORDER BY ";
			for (auto it = columns.begin(); it != columns.end(); it++)
			{
				this->query += (std::string)*it;
				if (std::next(it) != columns.end())
				{
					this->query += ", ";
				}
			}

			this->disabled |= this->masks[1];
		}

		return *this;
	}

	inline virtual select& limit(size_t limit)
	{
		if (this->is_disabled(2))
		{
			throw QueryError(
				"unable to set 'LIMIT' value, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->query += " LIMIT " + std::to_string(limit);
		this->disabled |= this->masks[2];
		return *this;
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
			this->query += " GROUP BY " + this->join_list(columns);
			this->disabled |= this->masks[3];
		}

		return *this;
	}

	inline virtual select& having(const q::condition& op)
	{
		if (this->is_disabled(4))
		{
			throw QueryError(
				"unable to set 'HAVING' condition, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->query += " HAVING " + (std::string)op;
		this->disabled |= this->masks[4];
		return *this;
	}

	inline virtual select& using_(DbDriver* database)
	{
		this->db = database;
		return *this;
	}

	inline virtual std::vector<std::shared_ptr<ModelT>> exec()
	{
		if (!this->db)
		{
			throw QueryError("select: database client not set", _ERROR_DETAILS_);
		}

		this->prepare_query();

		using row_t = std::map<const char*, char*>;
		using data_t = std::vector<std::shared_ptr<ModelT>>;
		data_t collection;
		this->db->run_select(this->query, &collection, [](void* container_ptr, void* row_ptr) -> void {
			auto& container = *(data_t *)container_ptr;
			auto& row = *(row_t *)row_ptr;
			auto model = std::make_shared<ModelT>();
			for (const auto& column : row)
			{
				if (column.second)
				{
					auto len = std::strlen(column.second);
					model->__set_attr__(column.first, {column.second, column.second + len + 1});
				}
			}

			container.push_back(model);
		});

		return collection;
	}

	[[nodiscard]]
	inline virtual std::string as_string()
	{
		this->prepare_query();
		return this->query;
	}
};

__Q_END__
