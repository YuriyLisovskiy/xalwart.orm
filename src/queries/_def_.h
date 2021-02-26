/**
 * query/_def_.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: queries module's definitions.
 */

#pragma once

// Module definitions.
#include "../_def_.h"


// xw::orm::q
#define __Q_BEGIN__ __ORM_BEGIN__ namespace q {
#define __Q_END__ } __ORM_END__

// xw::orm::q::utility
#define __Q_UTILITY_BEGIN__ __Q_BEGIN__ namespace utility {
#define __Q_UTILITY_END__ } __Q_END__
