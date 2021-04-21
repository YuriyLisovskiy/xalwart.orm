/**
 * queries/operations.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Set of SQL arithmetic and logical operations.
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../db/meta.h"
#include "../db/model.h"
#include "../utility.h"
#include "../exceptions.h"


__ORM_Q_BEGIN__

template <typename T>
concept column_field_iterator_type_c = std::is_fundamental_v<iterator_v_type<T>> ||
	std::is_same_v<std::string, iterator_v_type<T>> ||
	std::is_same_v<const char*, iterator_v_type<T>> ||
	std::is_same_v<dt::Date, iterator_v_type<T>> ||
	std::is_same_v<dt::Time, iterator_v_type<T>> ||
	std::is_same_v<dt::Datetime, iterator_v_type<T>>;

struct ordering final
{
private:
	std::string _table;
	std::string _column;
	bool _ascending = true;

public:
	inline ordering() = default;

	inline ordering(
		std::string table, std::string column, bool ascending
	) : _table(std::move(table)), _column(std::move(column)), _ascending(ascending)
	{
		if (this->_column.empty())
		{
			throw QueryError("ordering: 'column' is required", _ERROR_DETAILS_);
		}
	}

	inline explicit ordering(
		std::string table, std::string column
	) : _table(std::move(table)), _column(std::move(column))
	{
	}

	inline explicit operator std::string() const
	{
		auto result = util::quote_str(this->_column) + (this->_ascending ? " ASC" : " DESC");
		if (!this->_table.empty())
		{
			result = util::quote_str(this->_table) + "." + result;
		}

		return result;
	}
};

template <db::column_field_type_c ColumnT, db::model_based_type_c ModelT>
inline ordering asc(ColumnT ModelT::* column)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return ordering(ModelT::meta_table_name, db::get_column_name(column), true);
}

template <db::column_field_type_c ColumnT, db::model_based_type_c ModelT>
inline ordering desc(ColumnT ModelT::* column)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return ordering(ModelT::meta_table_name, db::get_column_name(column), false);
}

struct condition_t
{
public:
	// Condition string, example `"id" = 1`.
	std::string raw_condition;

public:
	inline condition_t() = default;

	inline explicit condition_t(std::string condition) : raw_condition(std::move(condition))
	{
	}

	inline explicit operator std::string() const
	{
		return this->raw_condition;
	};
};

struct column_condition_t : public condition_t
{
public:
	inline column_condition_t() = default;

	inline column_condition_t(
		const std::string& table_name,
		const std::string& column_name,
		const std::string& condition
	) : condition_t("")
	{
		if (column_name.empty())
		{
			throw QueryError("'column_name' is empty", _ERROR_DETAILS_);
		}

		this->raw_condition = util::quote_str(column_name) + " " + condition;
		if (!table_name.empty())
		{
			this->raw_condition = util::quote_str(table_name) + "." + this->raw_condition;
		}
	}
};

template <db::model_based_type_c ModelT, db::column_field_type_c ColumnT>
struct comparison_op_t : public column_condition_t
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

public:
	inline comparison_op_t() = default;

	inline explicit comparison_op_t(
		const std::string& column_name, const std::string& op, const ColumnT& value
	) : column_condition_t(
		ModelT::meta_table_name, column_name, op + " " + db::field_as_column_v(value)
	)
	{
	}
};

// SQL comparison operators for columns.
template <db::model_based_type_c ModelT, db::column_field_type_c ColumnT>
struct column_t
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

protected:
	// Name of given column.
	std::string name;

public:
	inline explicit column_t(std::string name) : name(std::move(name))
	{
	}

	comparison_op_t<ModelT, ColumnT> operator== (ColumnT val)
	{
		return comparison_op_t<ModelT, ColumnT>(this->name, "=", val);
	}

	comparison_op_t<ModelT, ColumnT> operator!= (ColumnT val)
	{
		return comparison_op_t<ModelT, ColumnT>(this->name, "!=", val);
	}

	comparison_op_t<ModelT, ColumnT> operator< (ColumnT val)
	{
		return comparison_op_t<ModelT, ColumnT>(this->name, "<", val);
	}

	comparison_op_t<ModelT, ColumnT> operator> (ColumnT val)
	{
		return comparison_op_t<ModelT, ColumnT>(this->name, ">", val);
	}

	comparison_op_t<ModelT, ColumnT> operator<= (ColumnT val)
	{
		return comparison_op_t<ModelT, ColumnT>(this->name, "<=", val);
	}

	comparison_op_t<ModelT, ColumnT> operator>= (ColumnT val)
	{
		return comparison_op_t<ModelT, ColumnT>(this->name, ">=", val);
	}
};

template <db::column_field_type_c ColumnT, db::model_based_type_c ModelT>
inline column_t<ModelT, ColumnT> c(ColumnT ModelT::* member_pointer)
{
	return column_t<ModelT, ColumnT>(db::get_column_name(member_pointer));
}

template <db::column_field_type_c ColumnT, db::model_based_type_c ModelT>
inline column_condition_t is_null(ColumnT ModelT::* member_pointer)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(ModelT::meta_table_name, db::get_column_name(member_pointer), "IS NULL");
}

template <db::column_field_type_c ColumnT, db::model_based_type_c ModelT>
inline column_condition_t is_not_null(ColumnT ModelT::* member_pointer)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(ModelT::meta_table_name, db::get_column_name(member_pointer), "IS NOT NULL");
}

// SQL logical operators.
inline condition_t operator& (const condition_t& left, const condition_t& right)
{
	return condition_t(
		"(" + (std::string)left + " AND " + (std::string)right + ")"
	);
}

inline condition_t operator| (const condition_t& left, const condition_t& right)
{
	return condition_t(
		"(" + (std::string)left + " OR " + (std::string)right + ")"
	);
}

inline condition_t operator~ (const condition_t& cond)
{
	return condition_t("NOT (" + (std::string)cond + ")");
}

template <db::column_field_type_c ColumnT, db::model_based_type_c ModelT>
inline column_condition_t between(ColumnT ModelT::* column, const ColumnT& lower, const ColumnT& upper)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, db::get_column_name(column),
		"BETWEEN " + db::field_as_column_v(lower) + " AND " + db::field_as_column_v(upper)
	);
}

template <db::model_based_type_c ModelT>
inline column_condition_t between(
	const char* ModelT::* column, const char* lower, const char* upper
)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, db::get_column_name(column),
		"BETWEEN " + db::field_as_column_v(lower) + " AND " + db::field_as_column_v(upper)
	);
}

template <db::model_based_type_c ModelT>
inline column_condition_t between(
	std::string ModelT::* column, const std::string& lower, const std::string& upper
)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, db::get_column_name(column),
		"BETWEEN " + db::field_as_column_v(lower) + " AND " + db::field_as_column_v(upper)
	);
}

template <db::column_field_type_c ColumnT, db::model_based_type_c ModelT>
inline column_condition_t like(ColumnT ModelT::* column, const std::string& pattern)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, db::get_column_name(column), "LIKE '" + pattern + "'"
	);
}

template <db::column_field_type_c ColumnT, db::model_based_type_c ModelT>
inline column_condition_t like(
	ColumnT ModelT::* column, const std::string& pattern, const std::string& escape
)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, db::get_column_name(column),
		"LIKE '" + pattern + "' ESCAPE '" + escape + "'"
	);
}

template <db::column_field_type_c ColumnT, column_field_iterator_type_c IteratorT, db::model_based_type_c ModelT>
inline column_condition_t in(ColumnT ModelT::* column, IteratorT begin, IteratorT end)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	if (begin == end)
	{
		throw QueryError("in: list is empty", _ERROR_DETAILS_);
	}

	std::string condition = "IN (" + str::join(
		", ", begin, end, [](const ColumnT& item) -> std::string { return db::field_as_column_v(item); }
	) + ")";
	return column_condition_t(ModelT::meta_table_name, db::get_column_name(column), condition);
}

template <db::column_field_type_c ColumnT, db::column_field_type_c RangeValueT, db::model_based_type_c ModelT>
inline column_condition_t in(ColumnT ModelT::* column, const std::initializer_list<RangeValueT>& values)
{
	return in<ColumnT, typename std::initializer_list<RangeValueT>::const_iterator, ModelT>(
		column, values.begin(), values.end()
	);
}

struct join_t
{
	std::string type;
	std::string table_name;
	q::condition_t condition;

	join_t(
		std::string type, std::string table_name, q::condition_t condition
	) : type(std::move(type)), table_name(std::move(table_name)), condition(std::move(condition))
	{
	}

	inline explicit operator std::string() const
	{
		return type + " JOIN \"" + table_name + "\" ON " + (std::string)condition;
	}
};

template <db::model_based_type_c LeftT, db::model_based_type_c RightT>
inline join_t join_on(
	const std::string& type, const std::string& fk_to_left="", const q::condition_t& extra_condition={}
)
{
	static_assert(LeftT::meta_table_name != nullptr, "'meta_table_name' of left model is not initialized");
	static_assert(RightT::meta_table_name != nullptr, "'meta_table_name' of right model is not initialized");

	std::string left_table_name = LeftT::meta_table_name;
	const auto& table_name = RightT::meta_table_name;
	auto fk = fk_to_left.empty() ? db::make_fk<LeftT>() : fk_to_left;
	auto left_pk = db::get_pk_name<LeftT>();
	if (left_pk.empty())
	{
		throw QueryError("join_on: model requires pk column", _ERROR_DETAILS_);
	}

	auto condition_str = util::quote_str(left_table_name) + "." + util::quote_str(left_pk)
		+ " = " +
		util::quote_str(table_name) + "." + util::quote_str(fk);
	auto extra_cond_str = (std::string)extra_condition;
	if (!extra_cond_str.empty())
	{
		condition_str += " AND (" + extra_cond_str + ")";
	}

	return join_t(type, table_name, q::condition_t(condition_str));
}

template <db::model_based_type_c LeftT, db::model_based_type_c RightT>
inline join_t inner_on(const std::string& fk_to_left="", const q::condition_t& extra_condition={})
{
	return join_on<LeftT, RightT>("INNER", fk_to_left, extra_condition);
}

template <db::model_based_type_c LeftT, db::model_based_type_c RightT>
inline join_t left_on(const std::string& fk_to_left="", const q::condition_t& extra_condition={})
{
	return join_on<LeftT, RightT>("LEFT", fk_to_left, extra_condition);
}

template <db::model_based_type_c LeftT, db::model_based_type_c RightT>
inline join_t cross_on(const std::string& fk_to_left="", const q::condition_t& extra_condition={})
{
	return join_on<LeftT, RightT>("CROSS", fk_to_left, extra_condition);
}

__ORM_Q_END__
