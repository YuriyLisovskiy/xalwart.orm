/**
 * driver.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./driver.h"

// Core libraries.
#include <xalwart.core/string_utils.h>


__ORM_BEGIN__

void SQLDriverBase::throw_empty_arg(
	const std::string& arg, int line, const char* function, const char* file
) const
{
	throw QueryError(
		this->name() + ": '" + arg + "' is required", line, function, file
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

	return "INSERT INTO " + util::quote_str(table_name) + " (" + columns + ") VALUES (" + values + ");";
}

std::string SQLDriverBase::make_select_query(
	const std::string& table_name,
	const std::initializer_list<const char*>& columns,
	bool distinct,
	const std::vector<q::join_t>& joins,
	const q::condition_t& where_cond,
	const std::initializer_list<q::ordering>& order_by_cols,
	long int limit,
	long int offset,
	const std::initializer_list<std::string>& group_by_cols,
	const q::condition_t& having_cond
) const
{
	if (table_name.empty())
	{
		this->throw_empty_arg("table_name", _ERROR_DETAILS_);
	}

	if (!columns.size())
	{
		this->throw_empty_arg("columns", _ERROR_DETAILS_);
	}

	auto raw_prefix = table_name + ".";
	auto prefix = util::quote_str(table_name) + ".";
	std::string columns_str;
	for (auto column = columns.begin(); column != columns.end(); column++)
	{
		auto column_str = std::string(*column);
		columns_str += prefix + util::quote_str(column_str);
		columns_str += " AS " + util::quote_str(raw_prefix + column_str);
		if (std::next(column) != columns.end())
		{
			columns_str += ", ";
		}
	}

	auto query = std::string("SELECT") + (distinct ? " DISTINCT" : "") + " " + columns_str + " FROM " + util::quote_str(table_name);

	for (const auto& join_row : joins)
	{
		query += " " + (std::string)join_row;
	}

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
			auto ob_column = *it;
			if (ob_column.column.find('.') == std::string::npos)
			{
				ob_column.column = prefix + util::quote_str(ob_column.column);
			}

			query += (std::string)ob_column;
			if (std::next(it) != order_by_cols.end())
			{
				query += ", ";
			}
		}
	}

	if (limit > -1)
	{
		query += " LIMIT " + std::to_string(limit);
	}

	if (offset > 0)
	{
		if (limit < 0)
		{
			throw QueryError(
				this->name() + ": 'offset' is used without 'limit'", _ERROR_DETAILS_
			);
		}

		query += " OFFSET " + std::to_string(offset);
	}

	if (group_by_cols.size())
	{
		query += " GROUP BY ";
		for (auto it = group_by_cols.begin(); it != group_by_cols.end(); it++)
		{
			auto gb_col = *it;
			if (gb_col.find('.') == std::string::npos)
			{
				query += prefix + util::quote_str(gb_col);
			}
			else
			{
				query += gb_col;
			}

			if (std::next(it) != group_by_cols.end())
			{
				query += ", ";
			}
		}
	}

	auto having_str = (std::string)having_cond;
	if (!having_str.empty())
	{
		if (!group_by_cols.size())
		{
			throw QueryError(
				this->name() + ": 'having' is used without 'group by'", _ERROR_DETAILS_
			);
		}

		query += " HAVING " + having_str;
	}

	return query + ";";
}

std::string SQLDriverBase::make_delete_query(
	const std::string& table_name, const q::condition_t& where_cond
) const
{
	if (table_name.empty())
	{
		this->throw_empty_arg("table_name", _ERROR_DETAILS_);
	}

	std::string query = "DELETE FROM " + util::quote_str(table_name);
	auto where_str = (std::string)where_cond;
	if (!where_str.empty())
	{
		query += " WHERE " + where_str;
	}

	return query + ";";
}

__ORM_END__
