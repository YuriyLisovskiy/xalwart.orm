/**
 * queries/tests_delete.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/update.h"

#include "./mocked_driver.h"

using namespace xw;


struct TestCase_Q_update_TestModel : public orm::Model<TestCase_Q_update_TestModel>
{
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "test";

	static const std::tuple<
		orm::column_meta_t<TestCase_Q_update_TestModel, int>,
		orm::column_meta_t<TestCase_Q_update_TestModel, std::string>
	> meta_columns;
};

const std::tuple<
	orm::column_meta_t<TestCase_Q_update_TestModel, int>,
	orm::column_meta_t<TestCase_Q_update_TestModel, std::string>
> TestCase_Q_update_TestModel::meta_columns = {
	orm::make_pk_column_meta("id", &TestCase_Q_update_TestModel::id),
	orm::make_column_meta("name", &TestCase_Q_update_TestModel::name)
};

TEST(TestCase_Q_update, constructor_ThrowsNullModel)
{
	TestCase_Q_update_TestModel model;
	model.mark_as_null();
	ASSERT_THROW(auto _ = orm::q::update(model), orm::QueryError);
}

TEST(TestCase_Q_update, query_ThrowsDriverIsNotSet)
{
	TestCase_Q_update_TestModel model;
	ASSERT_THROW(auto _ = orm::q::update(model).query(), orm::QueryError);
}

TEST(TestCase_Q_update, exec_ThrowsDriverIsNotSet)
{
	TestCase_Q_update_TestModel model;
	ASSERT_THROW(orm::q::update(model).exec(), orm::QueryError);
}

class TestCaseF_Q_update : public ::testing::Test
{
protected:
	MockedDriver* driver;

	void SetUp() override
	{
		this->driver = new MockedDriver();
	}

	void TearDown() override
	{
		delete this->driver;
	}
};

TEST_F(TestCaseF_Q_update, query_Success)
{
	TestCase_Q_update_TestModel model;
	model.id = 1;
	model.name = "John";

	auto expected = R"(UPDATE "test" SET name = 'John' WHERE "test"."id" = 1;)";
	auto actual = orm::q::update(model).use(this->driver).query();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_update, exec_NoThrow)
{
	TestCase_Q_update_TestModel model;
	model.id = 1;
	model.name = "John";

	ASSERT_NO_THROW(orm::q::update(model).use(this->driver).exec());
}
