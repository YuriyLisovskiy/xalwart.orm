/**
 * utility.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./utility.h"


__ORM_BEGIN__

template<>
struct item_return<int>{ typedef int type; };
template<>
int as<int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<double>{ typedef double type; };
template<>
double as<double>(const void* data)
{
	return std::stod((const char*)data);
}

template<>
struct item_return<std::string>{ typedef std::string type; };
template<>
std::string as<std::string>(const void* data)
{
	return std::string((const char*)data);
}

// TODO: add more types

__ORM_END__
