/**
 * queries/tests_delete.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/update.h"

#include "./mocked_driver.h"

using namespace xw;


struct TestCase_Q_update_TestModel : public orm::Model
{
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "test";

	inline static const std::tuple meta_columns = {
		orm::make_pk_column_meta("id", &TestCase_Q_update_TestModel::id),
		orm::make_column_meta("name", &TestCase_Q_update_TestModel::name)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_for<TestCase_Q_update_TestModel>(TestCase_Q_update_TestModel::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from<TestCase_Q_update_TestModel>(TestCase_Q_update_TestModel::meta_columns, attr_name);
	}
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

TEST(TestCase_Q_update, commit_one_ThrowsDriverIsNotSet)
{
	TestCase_Q_update_TestModel model;
	ASSERT_THROW(orm::q::update(model).commit_one(), orm::QueryError);
}

TEST(TestCase_Q_update, commit_batch_ThrowsDriverIsNotSet)
{
	TestCase_Q_update_TestModel model;
	ASSERT_THROW(orm::q::update(model).commit_batch(), orm::QueryError);
}

TEST(TestCase_Q_update, commit_one_ThrowsMultipleModelsWereSet)
{
	TestCase_Q_update_TestModel model_1, model_2;
	ASSERT_THROW(orm::q::update(model_1).model(model_2).commit_one(), orm::QueryError);
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

TEST_F(TestCaseF_Q_update, query_SingleRow)
{
	TestCase_Q_update_TestModel model;
	model.id = 1;
	model.name = "John";

	auto expected = R"(UPDATE "test" SET name = 'John' WHERE "test"."id" = 1;)";
	auto actual = orm::q::update(model).use(this->driver).query();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_update, query_MultipleRows)
{
	TestCase_Q_update_TestModel model_1;
	model_1.id = 1;
	model_1.name = "John";

	TestCase_Q_update_TestModel model_2;
	model_2.id = 2;
	model_2.name = "Steve";

	auto expected = R"(UPDATE "test" SET name = 'John' WHERE "test"."id" = 1; UPDATE "test" SET name = 'Steve' WHERE "test"."id" = 2;)";
	auto actual = orm::q::update(model_1).model(model_2).use(this->driver).query();
	ASSERT_EQ(expected, actual);
}

TEST_F(TestCaseF_Q_update, commit_one_NoThrow)
{
	TestCase_Q_update_TestModel model;
	model.id = 1;
	model.name = "John";

	ASSERT_NO_THROW(orm::q::update(model).use(this->driver).commit_one());
}

TEST_F(TestCaseF_Q_update, commit_batch_NoThrow)
{
	TestCase_Q_update_TestModel model_1;
	model_1.id = 1;
	model_1.name = "John";

	TestCase_Q_update_TestModel model_2;
	model_2.id = 2;
	model_2.name = "Steve";

	ASSERT_NO_THROW(orm::q::update(model_1).model(model_2).use(this->driver).commit_batch());
}
