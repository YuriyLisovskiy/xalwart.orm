/**
 * utility.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: ORM helpers.
 */

#pragma once

// Module definitions.
#include "./_def_.h"


__ORM_UTIL_BEGIN__

template<typename T>
struct item_return{ typedef T type; };

template<typename T>
typename item_return<T>::type as(const void*);

template<>
struct item_return<short int>{ typedef short int type; };
template<>
inline short int as<short int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<unsigned short int>{ typedef unsigned short int type; };
template<>
inline unsigned short int as<unsigned short int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<unsigned int>{ typedef unsigned int type; };
template<>
inline unsigned int as<unsigned int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<int>{ typedef int type; };
template<>
inline int as<int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<long int>{ typedef long int type; };
template<>
inline long int as<long int>(const void* data)
{
	return std::stol((const char*)data);
}

template<>
struct item_return<unsigned long int>{ typedef unsigned long int type; };
template<>
inline unsigned long int as<unsigned long int>(const void* data)
{
	return std::stoul((const char*)data);
}

template<>
struct item_return<long long int>{ typedef long long int type; };
template<>
inline long long int as<long long int>(const void* data)
{
	return std::stoll((const char*)data);
}

template<>
struct item_return<unsigned long long int>{ typedef unsigned long long int type; };
template<>
inline unsigned long long int as<unsigned long long int>(const void* data)
{
	return std::stoull((const char*)data);
}

template<>
struct item_return<float>{ typedef float type; };
template<>
inline float as<float>(const void* data)
{
	return std::stof((const char*)data);
}

template<>
struct item_return<double>{ typedef double type; };
template<>
inline double as<double>(const void* data)
{
	return std::stod((const char*)data);
}

template<>
struct item_return<long double>{ typedef long double type; };
template<>
inline long double as<long double>(const void* data)
{
	return std::stold((const char*)data);
}

template<>
struct item_return<std::string>{ typedef std::string type; };
template<>
inline std::string as<std::string>(const void* data)
{
	return std::string((const char*)data);
}

inline std::string quote_str(const std::string& s)
{
	return s.starts_with('"') ? s : '"' + s + '"';
}

__ORM_UTIL_END__
