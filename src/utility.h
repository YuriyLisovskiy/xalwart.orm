/**
 * utility.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "./_def_.h"


__ORM_BEGIN__

template<class T>
struct item_return{ typedef T type; };

template<class T>
typename item_return<T>::type as(const void*);

__ORM_END__
