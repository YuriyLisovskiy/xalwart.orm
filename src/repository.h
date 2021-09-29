/**
 * repository.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <memory>

// Base libraries.
#include <xalwart.base/abc/orm.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./backend.h"
#include "./transaction.h"


__ORM_BEGIN__

// TESTME: Repository
// TODO: docs for 'Repository'
// Requests the database connection on the first use,
// reuses it and returns it back on object destruction.
// Also, the connection can be returned manually by
// calling 'free_connection()' method.
class Repository
{
public:
	inline Repository() noexcept : sql_backend(nullptr), connection(nullptr)
	{
	}

	explicit inline Repository(abc::IBackend* backend)
	{
		this->sql_backend = dynamic_cast<abc::ISQLBackend*>(backend);
		this->check_state();
	}

	inline Repository(Repository&& other) noexcept
	{
		if (this != &other)
		{
			this->_move_from(std::forward<Repository>(other));
		}
	}

	inline Repository(const Repository& other) noexcept
	{
		if (this != &other)
		{
			this->_copy_from(other);
		}
	}

	inline ~Repository()
	{
		this->free_connection();
	}

	inline Repository& operator= (Repository&& other) noexcept
	{
		if (this != &other)
		{
			this->_move_from(std::forward<Repository>(other));
		}

		return *this;
	}

	inline Repository& operator= (const Repository& other) noexcept
	{
		if (this != &other)
		{
			this->_copy_from(other);
		}

		return *this;
	}

	inline void free_connection()
	{
		if (this->sql_backend && this->connection)
		{
			this->sql_backend->release_connection(this->connection);
			this->connection = nullptr;
		}
	}

	template <class T>
	inline q::Insert<T> insert()
	{
		this->ensure_connection();
		return q::Insert<T>(this->connection.get(), this->sql_backend->sql_builder());
	}

	template <class T>
	inline q::Select<T> select()
	{
		this->ensure_connection();
		return q::Select<T>(this->connection.get(), this->sql_backend->sql_builder());
	}

	template <class T>
	inline q::Update<T> update()
	{
		this->ensure_connection();
		return q::Update<T>(this->connection.get(), this->sql_backend->sql_builder());
	}

	template <class T>
	inline q::Delete<T> delete_()
	{
		this->ensure_connection();
		return q::Delete<T>(this->connection.get(), this->sql_backend->sql_builder());
	}

	inline Transaction transaction()
	{
		this->ensure_connection();
		return Transaction(this->connection.get(), this->sql_backend->sql_builder());
	}

protected:
	abc::ISQLBackend* sql_backend = nullptr;
	std::shared_ptr<abc::IDatabaseConnection> connection = nullptr;

	inline void check_state() const
	{
		require_non_null(
			this->sql_backend,
			"Backend is not derived from 'xw::orm::abc::ISQLBackend' or is nullptr",
			_ERROR_DETAILS_
		);
	}

	inline void ensure_connection()
	{
		this->check_state();
		if (!this->connection)
		{
			this->connection = this->sql_backend->get_connection();
		}
	}

private:
	inline void _copy_from(const Repository& other) noexcept
	{
		this->sql_backend = other.sql_backend;
		this->connection = other.connection;
	}

	inline void _move_from(Repository&& other) noexcept
	{
		this->sql_backend = other.sql_backend;
		other.sql_backend = nullptr;

		this->connection = std::move(other.connection);
		other.connection = nullptr;
	}
};

__ORM_END__
