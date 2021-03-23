/**
 * tests_model.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../src/model.h"

using namespace xw;

class TestCase_Model_TestModel : public orm::Model<TestCase_Model_TestModel>
{
public:
	int id{};
	std::string name;
	const char* info;

	static const std::tuple<
		orm::column_meta_t<TestCase_Model_TestModel, int>,
		orm::column_meta_t<TestCase_Model_TestModel, std::string>,
		orm::column_meta_t<TestCase_Model_TestModel, const char*>
	> meta_columns;
};

const std::tuple<
	orm::column_meta_t<TestCase_Model_TestModel, int>,
	orm::column_meta_t<TestCase_Model_TestModel, std::string>,
	orm::column_meta_t<TestCase_Model_TestModel, const char*>
> TestCase_Model_TestModel::meta_columns = {
	orm::make_pk_column_meta("id", &TestCase_Model_TestModel::id),
	orm::make_column_meta("name", &TestCase_Model_TestModel::name),
	orm::make_column_meta("info", &TestCase_Model_TestModel::info)
};

TEST(TestCase_Model, __cmp___throws_NotImplementedException)
{
	auto left = TestCase_Model_TestModel();
	auto right = TestCase_Model_TestModel();
	ASSERT_THROW(auto res = left.__cmp__(&right), core::NotImplementedException);
}

TEST(TestCase_Model, is_null_False)
{
	TestCase_Model_TestModel model;
	ASSERT_FALSE(model.is_null());
}

TEST(TestCase_Model, is_null_True)
{
	TestCase_Model_TestModel model;
	model.mark_as_null();
	ASSERT_TRUE(model.is_null());
}

TEST(TestCase_Model, __str___NullModel)
{
	TestCase_Model_TestModel model;
	model.mark_as_null();
	ASSERT_EQ(model.__str__(), "null");
}

TEST(TestCase_Model, __repr___NullModel)
{
	TestCase_Model_TestModel model;
	model.mark_as_null();
	ASSERT_EQ(model.__repr__(), "null");
}

TEST(TestCase_Model, meta_table_name_Default_IsNullptr)
{
	ASSERT_EQ(TestCase_Model_TestModel::meta_table_name, nullptr);
}

TEST(TestCase_Model, meta_omit_pk_Default_IsTrue)
{
	ASSERT_TRUE(TestCase_Model_TestModel::meta_omit_pk);
}

TEST(TestCase_Model, get_column_value_as_string_GetNum)
{
	auto model = TestCase_Model_TestModel();
	model.id = 10;
	ASSERT_EQ(orm::get_column_value_as_string(
		model, std::get<0>(TestCase_Model_TestModel::meta_columns)
	), "10");
}

TEST(TestCase_Model, get_column_value_as_string_GetString)
{
	auto model = TestCase_Model_TestModel();
	model.name = "Steve";
	ASSERT_EQ(orm::get_column_value_as_string(
		model, std::get<1>(TestCase_Model_TestModel::meta_columns)
	), "'Steve'");
}

TEST(TestCase_Model, get_column_value_as_string_GetCString)
{
	auto model = TestCase_Model_TestModel();
	model.info = "NoNe";
	ASSERT_EQ(orm::get_column_value_as_string(
		model, std::get<2>(TestCase_Model_TestModel::meta_columns)
	), "'NoNe'");
}
