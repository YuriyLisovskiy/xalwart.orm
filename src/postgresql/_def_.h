/**
 * postgresql/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Definitions of 'postgresql' module.
 */

#pragma once

#ifdef USE_POSTGRESQL

// Module definitions.
#include "../_def_.h"

// xw::orm::postgresql
#define __ORM_POSTGRESQL_BEGIN__ __ORM_BEGIN__ namespace postgresql {
#define __ORM_POSTGRESQL_END__ } __ORM_END__

#endif // USE_POSTGRESQL
