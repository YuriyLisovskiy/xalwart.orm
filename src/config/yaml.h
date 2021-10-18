/**
 * config/yaml.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <map>
#include <string>
#include <memory>

// Base libraries.
#include <xalwart.base/config/components/yaml/default.h>
#include <xalwart.base/abc/orm.h>
#include <xalwart.base/path.h>

// Module definitions.
#include "./_def_.h"


__ORM_CONFIG_BEGIN__

// TESTME: parse_scalar
// TODO: docs for 'parse_scalar'
extern std::string _parse_scalar(const std::string& name, const YAML::Node& node);

// TESTME: YAMLDatabasesComponent
// TODO: docs for 'YAMLDatabasesComponent'
class YAMLDatabasesComponent : public xw::config::YAMLSequenceComponent
{
public:
	explicit inline YAMLDatabasesComponent(
		path::Path base_directory, std::map<std::string, std::shared_ptr<abc::IBackend>>& backends
	) : YAMLSequenceComponent(
		[this](const YAML::Node& node)
		{
			auto name = _parse_scalar("name", node);
			auto dbms = _parse_scalar("dbms", node);
			this->handle_database(dbms, name, node);
		}
	), base_directory(std::move(base_directory)), backends(backends)
	{
	}

protected:
	path::Path base_directory;
	std::map<std::string, std::shared_ptr<abc::IBackend>>& backends;

	void handle_database(const std::string& dbms, const std::string& name, const YAML::Node& node);
};

__ORM_CONFIG_END__
