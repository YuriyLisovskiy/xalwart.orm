/**
 * sqlite3/config/yaml.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./yaml.h"

#ifdef USE_SQLITE3

// Base libraries.
#include <xalwart.base/exceptions.h>
#include <xalwart.base/path.h>

// Orm libraries.
#include "../backend.h"


__ORM_SQLITE3_BEGIN__

void YAMLSQLite3Component::initialize(const YAML::Node& node) const
{
	xw::config::YAMLMapComponent::initialize(node);
	if (this->filename.empty())
	{
		throw ImproperlyConfigured(
			"'file' of sqlite3 database configuration should be non-empty string",
			_ERROR_DETAILS_
		);
	}

	if (this->pool_size < 1)
	{
		throw ImproperlyConfigured(
			"'connections' should be positive integer", _ERROR_DETAILS_
		);
	}

	auto full_filename = path::Path(this->filename);
	if (!path::Path(full_filename).is_absolute())
	{
		full_filename = this->base_directory / full_filename;
	}

	this->backend = std::make_shared<sqlite3::Backend>(
		this->pool_size, full_filename.to_string().c_str()
	);
	this->backend->create_pool();
}

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
