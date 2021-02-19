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
#include <vector>
#include <string>
#include <iostream>

// SQLite
#include <sqlite3.h>

// Core libraries.
#include <xalwart.core/string_utils.h>
#include <xalwart.core/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../q.h"
#include "../exceptions.h"


__SQLITE3_BEGIN__

template <typename T, typename = int>
struct has_meta_table_name : std::false_type { };

template <typename T>
struct has_meta_table_name <T, decltype((void) T::Meta::table_name, 0)> : std::true_type { };

template <typename ModelT>
class SelectQuery
{
protected:
	::sqlite3* db;

	std::string query;
	std::string table_name;
	bool distinct;
	bool is_limited;

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

public:
	inline explicit SelectQuery(::sqlite3* db) : db(db), distinct(false), is_limited(false)
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

	inline SelectQuery& where(const q::operator_base& op)
	{
		if (this->is_limited)
		{
			throw SQLError("method 'limit' must be called last", _ERROR_DETAILS_);
		}

		this->query += " WHERE " + (std::string)op;
		return *this;
	}

	inline SelectQuery& having()
	{
		if (this->is_limited)
		{
			throw SQLError("method 'limit' must be called last", _ERROR_DETAILS_);
		}

		// TODO: having
		return *this;
	}

	inline SelectQuery& order_by(const std::initializer_list<std::string>& columns)
	{
		if (this->is_limited)
		{
			throw SQLError("method 'limit' must be called last", _ERROR_DETAILS_);
		}

		if (columns.size())
		{
			auto result = this->join_list(columns);
			if (!result.empty())
			{
				this->query += " ORDER BY " + result;
			}
		}

		return *this;
	}

	inline SelectQuery& group_by(const std::initializer_list<std::string>& columns)
	{
		if (this->is_limited)
		{
			throw SQLError("method 'limit' must be called last", _ERROR_DETAILS_);
		}

		if (columns.size())
		{
			auto result = this->join_list(columns);
			if (!result.empty())
			{
				this->query += " GROUP BY " + result;
			}
		}

		return *this;
	}

	inline SelectQuery& limit(size_t limit)
	{
		if (!this->is_limited)
		{
			this->query += " LIMIT " + std::to_string(limit);
			this->is_limited = true;
		}

		return *this;
	}

	inline std::vector<std::shared_ptr<ModelT>> exec()
	{
		this->prepare_query();
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
