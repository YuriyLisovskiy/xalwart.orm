/**
 * query/insert.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
// TODO

// Core libraries.
#include <xalwart.core/types/string.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../model.h"
#include "../driver.h"

__Q_BEGIN__

template <ModelBasedType ModelT>
class insert
{
protected:
	std::string columns_str;
	std::string query;
	std::string rows;

	DbDriver* db = nullptr;

protected:
	virtual inline void append_model(const ModelT& model, bool is_first)
	{
		this->rows += is_first ? "(" : ", (";
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
				this->rows += value->__repr__();
			}
			else
			{
				this->rows += value->__str__();
			}

			if (std::next(attr) != model.attrs_end())
			{
				if (is_first)
				{
					this->columns_str += ", ";
				}

				this->rows += ", ";
			}
		}

		this->rows += ")";
	}

public:
	inline explicit insert(const ModelT& model)
	{
		this->query = "INSERT INTO ";
		if constexpr (ModelT::meta_table_name == nullptr)
		{
			this->query += std::string(ModelT::meta_table_name);
		}
		else
		{
			auto table_name = utility::demangle(typeid(ModelT).name());
			this->query += table_name.substr(table_name.rfind(':') + 1);
		}

		this->append_model(model, true);
		this->query += " (" + this->columns_str + ") VALUES";
	};

	inline explicit insert(DbDriver* driver, const ModelT& model) : insert(model)
	{
		this->db = driver;
	};

	virtual inline insert& values(const ModelT& model)
	{
		this->append_model(model, false);
		return *this;
	}

	inline virtual void exec()
	{
		if (!this->db)
		{
			throw QueryError("insert: database client not set", _ERROR_DETAILS_);
		}

		auto full_query = this->query + " " + this->rows;

		// TODO: perform insert
	}

	[[nodiscard]]
	virtual inline std::string as_string() const
	{
		return this->query + " " + this->rows;
	}
};

__Q_END__
