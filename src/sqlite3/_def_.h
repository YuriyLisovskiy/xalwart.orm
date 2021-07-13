/**
 * sqlite3/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Definitions of `sqlite3` module.
 */

#pragma once

#ifdef USE_SQLITE3

// Module definitions.
#include "../_def_.h"

// xw::orm::sqlite3
#define __ORM_SQLITE3_BEGIN__ __ORM_BEGIN__ namespace sqlite3 {
#define __ORM_SQLITE3_END__ } __ORM_END__

#endif // USE_SQLITE3
