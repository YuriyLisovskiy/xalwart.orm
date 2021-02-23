/**
 * sqlite3/query.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

#ifdef USE_SQLITE3

// C++ libraries.
#include <string>
#include <iostream>

// SQLite
#include <sqlite3.h>

// Core libraries.
#include <xalwart.core/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../q.h"
#include "../model.h"
#include "../exceptions.h"


__SQLITE3_BEGIN__

template <typename T, typename = int>
struct has_meta_table_name : std::false_type { };

template <typename T>
struct has_meta_table_name <T, decltype((void) T::Meta::table_name, 0)> : std::true_type { };

template <ModelBasedType ModelT>
class SelectQuery
{
protected:
	// Database connection, TODO: replace it with connection interface.
	::sqlite3* db;

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
	inline void prepare_query()
	{
		this->query = std::string("SELECT") + (this->distinct ? " DISTINCT" : "") + " * FROM " + this->table_name + this->query + ';';
	}

	inline std::string join_list(const std::initializer_list<std::string>& columns)
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

	inline bool is_disabled(uint item)
	{
		return (this->disabled >> item) & 1UL;
	}

public:
	inline explicit SelectQuery(::sqlite3* db) : db(db)
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

	inline SelectQuery& where(const q::condition& op)
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

	inline SelectQuery& order_by(const std::initializer_list<q::ordering>& columns)
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

	inline SelectQuery& limit(size_t limit)
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

	inline SelectQuery& group_by(const std::initializer_list<std::string>& columns)
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

	inline SelectQuery& having(const q::condition& op)
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

	inline std::vector<std::shared_ptr<ModelT>> exec()
	{
		this->prepare_query();

		// TODO: remove
		std::cerr << this->query << '\n';

		char* message_error;
		using data_t = std::vector<std::shared_ptr<ModelT>>;
		data_t models;
		auto res = sqlite3_exec(
			this->db,
			this->query.c_str(),
			[](void* data, int argc, char** argv, char** column_names) -> int {
				auto& container = *(data_t*)data;
				auto model = std::make_shared<ModelT>();
				for (int i = 0; i < argc; i++)
				{
					auto len = std::strlen(argv[i]);
					if (argv[i])
					{
						model->__set_attr__(column_names[i], {argv[i], argv[i] + len + 1});
					}
				}

				container.push_back(model);
				return 0;
			},
			&models,
			&message_error
		);

		if (res != SQLITE_OK)
		{
			auto message = std::string(message_error);
			sqlite3_free(message_error);
			throw core::RuntimeError(message, _ERROR_DETAILS_);
		}

		return models;
	}
};

__SQLITE3_END__

#endif // USE_SQLITE3
