/**
 * postgresql/credentials.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./credentials.h"

#ifdef USE_POSTGRESQL

// Orm libraries.
#include "../exceptions.h"


__ORM_POSTGRESQL_BEGIN__

void PostgreSQLCredentials::validate() const
{
	if (this->name.empty())
	{
		throw DatabaseError("PostgreSQL database name is empty", _ERROR_DETAILS_);
	}

	if (this->user.empty())
	{
		throw DatabaseError("PostgreSQL database user is empty", _ERROR_DETAILS_);
	}

	if (this->password.empty())
	{
		throw DatabaseError("PostgreSQL database password is empty", _ERROR_DETAILS_);
	}

	if (this->host.empty())
	{
		throw DatabaseError("PostgreSQL database host is empty", _ERROR_DETAILS_);
	}
}

__ORM_POSTGRESQL_END__

#endif // USE_POSTGRESQL
