/**
 * sqlite3/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: sqlite3 module's definitions.
 */

#pragma once

#ifdef USE_SQLITE3

// Module definitions.
#include "../_def_.h"


// xw::orm::sqlite3
#define __SQLITE3_BEGIN__ __ORM_BEGIN__ namespace sqlite3 {
#define __SQLITE3_END__ } __ORM_END__

#endif // USE_SQLITE3
