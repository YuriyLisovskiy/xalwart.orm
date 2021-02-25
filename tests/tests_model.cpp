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

TEST(TestCase_Model, is_null_False)
{
	orm::Model model;
	ASSERT_FALSE(model.is_null());
}

TEST(TestCase_Model, is_null_True)
{
	orm::Model model;
	model.mark_as_null();
	ASSERT_TRUE(model.is_null());
}

TEST(TestCase_Model, __str___NullModel)
{
	orm::Model model;
	model.mark_as_null();
	ASSERT_EQ(model.__str__(), "Model{null}");
}

TEST(TestCase_Model, __repr___NullModel)
{
	orm::Model model;
	model.mark_as_null();
	ASSERT_EQ(model.__repr__(), "Model{null}");
}

TEST(TestCase_Model, meta_table_name_Default_IsNullptr)
{
	ASSERT_EQ(orm::Model::meta_table_name, nullptr);
}

TEST(TestCase_Model, meta_pk_name_Default_IsId)
{
	ASSERT_EQ(orm::Model::meta_pk_name, "id");
}

TEST(TestCase_Model, meta_omit_pk_Default_IsTrue)
{
	ASSERT_TRUE(orm::Model::meta_omit_pk);
}