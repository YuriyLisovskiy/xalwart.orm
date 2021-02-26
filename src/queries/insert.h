/**
 * query/insert.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// Core libraries.
#include <xalwart.core/types/string.h>
#include <xalwart.core/object/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../model.h"
#include "../abc.h"
#include "../exceptions.h"


__Q_BEGIN__

template <ModelBasedType ModelT>
class insert
{
protected:
	abc::ISQLDriver* db = nullptr;

	std::string table_name;
	std::string columns_str;
	std::vector<std::string> rows;

	bool is_bulk = false;

protected:
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
	inline explicit insert(const ModelT& model)
	{
		if constexpr (ModelT::meta_table_name != nullptr)
		{
			this->table_name += std::string(ModelT::meta_table_name);
		}
		else
		{
			this->table_name = utility::demangle(typeid(ModelT).name());
			this->table_name = this->table_name.substr(this->table_name.rfind(':') + 1);
		}

		this->append_model(model, true);
	};

	inline explicit insert(abc::ISQLDriver* driver, const ModelT& model) : insert(model)
	{
		this->db = driver;
	};

	virtual inline insert& model(const ModelT& model)
	{
		this->is_bulk = true;
		this->append_model(model, false);
		return *this;
	}

	inline virtual insert& using_(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->db = driver;
		}

		return *this;
	}

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

	template <typename T>
	inline void one(T& pk) const
	{
		pk = object::as<T>(this->one().c_str());
	}

	inline virtual void bulk()
	{
		auto query = this->query();
		this->db->run_insert(query, true);
	}

	[[nodiscard]]
	virtual inline std::string query() const
	{
		if (!this->db)
		{
			throw QueryError("insert: database client not set", _ERROR_DETAILS_);
		}

		return this->db->make_insert_query(
			this->table_name, this->columns_str, this->rows
		);
	}
};

__Q_END__
