/**
 * repository.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./repository.h"

// STL libraries.
#include <iostream>
#include <exception>


__ORM_BEGIN__

void Repository::transaction(const std::function<void(Transaction&)>& func)
{
	this->ensure_connection();
	this->wrap([&](auto*)
	{
		auto tr = Transaction(this->connection.get(), this->sql_backend->sql_builder());
		try
		{
			func(tr);
			tr.rollback();
		}
		catch (const std::exception& exc)
		{
			tr.rollback();
			throw;
		}
	});
}

void Repository::wrap(const std::function<void(Repository*)>& func)
{
	this->ensure_connection();
	try
	{
		func(this);
		this->free_connection();
	}
	catch (const std::exception& exc)
	{
		this->free_connection();
		throw;
	}
}

__ORM_END__
