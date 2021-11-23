/**
 * postgresql/config/yaml.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./yaml.h"

#ifdef USE_POSTGRESQL

// Base libraries.
#include <xalwart.base/exceptions.h>

// Orm libraries.
#include "../backend.h"


__ORM_POSTGRESQL_BEGIN__

void YAMLPostgreSQLComponent::initialize(const YAML::Node& node) const
{
	xw::config::YAMLMapComponent::initialize(node);
	try
	{
		this->credentials.validate();
	}
	catch (const DatabaseError& exc)
	{
		throw ImproperlyConfigured(exc.what(), _ERROR_DETAILS_);
	}

	if (this->pool_size < 1)
	{
		throw ImproperlyConfigured(
			"'connections' should be positive integer", _ERROR_DETAILS_
		);
	}

	this->backend = std::make_shared<Backend>(this->pool_size, this->credentials);
	this->backend->create_pool();
}

__ORM_POSTGRESQL_END__

#endif // USE_POSTGRESQL
