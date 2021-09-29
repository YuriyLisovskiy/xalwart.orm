/**
 * sqlite3/config/yaml.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

#ifdef USE_SQLITE3

// C++ libraries.
#include <string>
#include <memory>

// Base libraries.
#include <xalwart.base/config/components/yaml/default.h>
#include <xalwart.base/vendor/yaml/yaml-cpp/yaml.h>
#include <xalwart.base/abc/orm.h>

// Module definitions.
#include "../_def_.h"


__ORM_SQLITE3_BEGIN__

// TESTME: YAMLSQLite3Component
// TODO: docs for 'YAMLSQLite3Component'
class YAMLSQLite3Component : public xw::config::YAMLMapComponent
{
public:
	explicit YAMLSQLite3Component(
		std::string base_directory, std::shared_ptr<abc::IBackend>& backend
	) : base_directory(std::move(base_directory)), backend(backend)
	{
		this->register_component("file", std::make_unique<xw::config::YAMLScalarComponent>(this->filename));
		this->register_component("connections", std::make_unique<xw::config::YAMLScalarComponent>(this->pool_size));
	}

	void initialize(const YAML::Node& node) const override;

protected:
	std::string base_directory;
	std::shared_ptr<abc::IBackend>& backend;

	std::string filename;
	long pool_size = 3;
};

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
