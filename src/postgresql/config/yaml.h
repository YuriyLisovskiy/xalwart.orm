/**
 * postgresql/config/yaml.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

#ifdef USE_POSTGRESQL

// C++ libraries.
#include <string>
#include <memory>

// Base libraries.
#include <xalwart.base/config/components/yaml/default.h>
#include <xalwart.base/interfaces/orm.h>

// Module definitions.
#include "../_def_.h"

// Orm libraries.
#include "../credentials.h"


__ORM_POSTGRESQL_BEGIN__

// TESTME: YAMLPostgreSQLComponent
// TODO: docs for 'YAMLPostgreSQLComponent'
class YAMLPostgreSQLComponent : public xw::config::YAMLMapComponent
{
public:
	explicit YAMLPostgreSQLComponent(std::shared_ptr<IBackend>& backend) : backend(backend)
	{
		this->register_component("name", std::make_unique<xw::config::YAMLScalarComponent>(this->credentials.name));
		this->register_component("user", std::make_unique<xw::config::YAMLScalarComponent>(this->credentials.user));
		this->register_component(
			"password", std::make_unique<xw::config::YAMLScalarComponent>(this->credentials.password)
		);
		this->register_component("host", std::make_unique<xw::config::YAMLScalarComponent>(this->credentials.host));
		this->register_component("port", std::make_unique<xw::config::YAMLScalarComponent>(this->credentials.port));
		this->register_component("connections", std::make_unique<xw::config::YAMLScalarComponent>(this->pool_size));
	}

	void initialize(const YAML::Node& node) const override;

protected:
	std::shared_ptr<IBackend>& backend;

	PostgreSQLCredentials credentials;
	long pool_size = 3;
};

__ORM_POSTGRESQL_END__

#endif // USE_POSTGRESQL
