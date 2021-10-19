/**
 * repository.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./repository.h"

// STL libraries.
#include <iostream>

// ORM libraries.
#include "./exceptions.h"


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
		catch (const SQLError& exc)
		{
			tr.rollback();
			throw exc;
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
	catch (const SQLError& exc)
	{
		this->free_connection();
		throw exc;
	}
}

__ORM_END__
