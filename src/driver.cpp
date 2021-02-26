/**
 * driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./driver.h"

// C++ libraries.
// TODO

// Core libraries.
#include <xalwart.core/string_utils.h>

// Orm libraries.
#include "./exceptions.h"


__ORM_BEGIN__

void SQLDriverBase::throw_empty_arg(
	const std::string& arg, int line, const char* function, const char* file
) const
{
	throw QueryError(
		this->name() + ": non-empty '" + arg + "' is required", line, function, file
	);
}

std::string SQLDriverBase::make_insert_query(
	const std::string& table_name,
	const std::string& columns,
	const std::vector<std::string>& rows
) const
{
	if (table_name.empty())
	{
		this->throw_empty_arg("table_name", _ERROR_DETAILS_);
	}

	if (columns.empty())
	{
		this->throw_empty_arg("columns", _ERROR_DETAILS_);
	}

	auto values = str::join(rows.begin(), rows.end(), "), (");
	if (values.empty())
	{
		this->throw_empty_arg("rows", _ERROR_DETAILS_);
	}

	return "INSERT INTO " + table_name + " (" + columns + ") VALUES (" + values + ");";
}

std::string SQLDriverBase::make_select_query(
	const std::string& table_name,
	bool distinct,
	const q::condition& where_cond,
	const std::initializer_list<q::ordering>& order_by_cols,
	long int limit,
	long int offset,
	const std::initializer_list<std::string>& group_by_cols,
	const q::condition& having_cond
) const
{
	if (table_name.empty())
	{
		this->throw_empty_arg("table_name", _ERROR_DETAILS_);
	}

	auto query = std::string("SELECT") + (distinct ? " DISTINCT" : "") + " * FROM " + table_name;

	auto where_str = (std::string)where_cond;
	if (!where_str.empty())
	{
		query += " WHERE " + where_str;
	}

	if (order_by_cols.size())
	{
		query += " ORDER BY ";
		for (auto it = order_by_cols.begin(); it != order_by_cols.end(); it++)
		{
			query += (std::string)*it;
			if (std::next(it) != order_by_cols.end())
			{
				query += ", ";
			}
		}
	}

	if (limit > -1)
	{
		query += " LIMIT " + std::to_string(limit);
		if (offset > 0)
		{
			query += " OFFSET " + std::to_string(offset);
		}
	}

	if (group_by_cols.size())
	{
		query += " GROUP BY " + str::join(group_by_cols.begin(), group_by_cols.end(), ", ");
	}

	auto having_str = (std::string)having_cond;
	if (!having_str.empty())
	{
		query += " HAVING " + having_str;
	}

	return query + ";";
}

__ORM_END__
