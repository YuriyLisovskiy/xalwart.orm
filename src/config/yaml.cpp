/**
 * config/yaml.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./yaml.h"

// Base libraries.
#include <xalwart.base/exceptions.h>

// Orm libraries.
#include "../sqlite3/config/yaml.h"


__ORM_CONFIG_BEGIN__

std::string _parse_scalar(const std::string& name, const YAML::Node& node)
{
	std::string value;
	auto scalar_component = xw::config::YAMLScalarComponent(value);
	scalar_component.initialize(node[name]);
	if (value.empty())
	{
		throw ImproperlyConfigured("'" + name + "' should be non-empty string", _ERROR_DETAILS_);
	}

	return value;
}

void YAMLDatabasesComponent::handle_database(const std::string& dbms, const std::string& name, const YAML::Node& node)
{
	if (this->backends.contains(name))
	{
		throw ImproperlyConfigured(
			"Database name should be unique, found duplicated name '" + name + "'", _ERROR_DETAILS_
		);
	}

	if (dbms == "sqlite3")
	{
		std::shared_ptr<abc::orm::Backend> backend = nullptr;
		auto component = sqlite3::YAMLSQLite3Component(this->base_directory, backend);
		component.initialize(node);
		if (backend != nullptr)
		{
			this->backends.insert(std::make_pair(name, std::move(backend)));
		}
	}
	else
	{
		// TODO: add custom database loading
		throw ImproperlyConfigured("Unsupported database '" + dbms + "'", _ERROR_DETAILS_);
	}
}

__ORM_CONFIG_END__
