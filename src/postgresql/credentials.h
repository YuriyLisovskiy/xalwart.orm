/**
 * postgresql/credentials.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 */

#pragma once

#ifdef USE_POSTGRESQL

// STL
#include <string>

// Module definitions.
#include "./_def_.h"


__ORM_POSTGRESQL_BEGIN__

struct PostgreSQLCredentials final
{
	std::string name;
	std::string user;
	std::string password;
	std::string host = "localhost";
	unsigned int port = 5432;

	void validate() const;
};

__ORM_POSTGRESQL_END__

#endif // USE_POSTGRESQL
