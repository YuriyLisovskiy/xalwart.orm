/**
 * tests_model.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/model.h"

using namespace xw;

TEST(TestCase_Model, __cmp___throws_NotImplementedException)
{
	auto left = orm::Model();
	auto right = orm::Model();
	ASSERT_THROW(auto res = left.__cmp__(&right), core::NotImplementedException);
}
