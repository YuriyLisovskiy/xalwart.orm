/**
 * queries/insert.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: wrapper for SQL 'INSERT' statement.
 */

#pragma once

// Core libraries.
#include <xalwart.core/types/string.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "../exceptions.h"


__Q_BEGIN__

template <ModelBasedType ModelT>
class insert
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");
	static_assert(ModelT::meta_pk_name != nullptr, "'meta_pk_name' is not initialized");

protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* db = nullptr;

	// Holds columns names.
	// Generates during the first model appending.
	std::string columns_str;

	// Collection of rows to insert.
	std::vector<std::string> rows;

	// Mark if bulk insert is used.
	bool is_bulk = false;

protected:

	// Converts model into row (string) and appends it to `rows`.
	//
	// `is_first`: used to indicate if `append` is called for the
	// first time. If true, than generates `columns_str`.
	virtual inline void append_model(const ModelT& model, bool is_first)
	{
		std::string row;
		for (auto attr = model.attrs_begin(); attr != model.attrs_end(); attr++)
		{
			if constexpr (ModelT::meta_omit_pk)
			{
				if (attr->first == ModelT::meta_pk_name)
				{
					continue;
				}
			}

			if (is_first)
			{
				this->columns_str += attr->first;
			}

			auto value = attr->second.get();
			if (dynamic_cast<types::String*>(value.get()))
			{
				row += value->__repr__();
			}
			else
			{
				row += value->__str__();
			}

			if (std::next(attr) != model.attrs_end())
			{
				if (is_first)
				{
					this->columns_str += ", ";
				}

				row += ", ";
			}
		}

		this->rows.push_back(row);
	}

public:

	// Appends model to insertion list.
	//
	// Throws 'QueryError' if model is null.
	inline explicit insert(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError(
				"insert: unable to insert null model", _ERROR_DETAILS_
			);
		}

		this->append_model(model, true);
	};

	// Sets SQL driver and appends model to insertion list.
	inline explicit insert(abc::ISQLDriver* driver, const ModelT& model) : insert(model)
	{
		this->db = driver;
	};

	// Sets SQL driver.
	inline virtual insert& use(abc::ISQLDriver* driver)
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
	virtual inline std::string query() const
	{
		if (!this->db)
		{
			throw QueryError("insert: database driver not set", _ERROR_DETAILS_);
		}

		return this->db->make_insert_query(
			get_table_name<ModelT>(), this->columns_str, this->rows
		);
	}

	// Appends model to insertion list.
	//
	// Throws 'QueryError' if model is null.
	virtual inline insert& model(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError(
				"insert: unable to insert null model", _ERROR_DETAILS_
			);
		}

		this->is_bulk = true;
		this->append_model(model, false);
		return *this;
	}

	// Inserts one row and returns inserted pk as string.
	//
	// Throws 'QueryError' if more than one model was set.
	[[nodiscard]]
	inline virtual std::string one() const
	{
		if (this->is_bulk)
		{
			throw QueryError(
				"insert: unable to return inserted model, trying to insert multiple models",
				_ERROR_DETAILS_
			);
		}

		auto query = this->query();
		return this->db->run_insert(query, false);
	}

	// Inserts one row and sets inserted primary key
	// to `pk` as type T.
	template <typename T>
	inline void one(T& pk) const
	{
		pk = util::as<T>(this->one().c_str());
	}

	// Inserts row(s) into database.
	inline virtual void bulk()
	{
		auto query = this->query();
		this->db->run_insert(query, true);
	}
};

__Q_END__
