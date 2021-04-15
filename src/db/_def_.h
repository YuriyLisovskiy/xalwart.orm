/**
 * db/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Definitions of `db` module.
 */

#pragma once

// Module definitions.
#include "../_def_.h"


// xw::orm::db
#define __DB_BEGIN__ __ORM_BEGIN__ namespace db {
#define __DB_END__ } __ORM_END__

// xw::orm::db::abc
#define __DB_ABC_BEGIN__ __DB_BEGIN__ namespace abc {
#define __DB_ABC_END__ } __DB_END__
