/**
 * queries/tests_delete.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/delete.h"

using namespace xw;

struct TestCaseF_Q_delete_TestModel : public orm::Model<TestCaseF_Q_delete_TestModel>
{
	int id{};
	std::string name;

	static constexpr const char* meta_table_name = "test_models";

	static const std::tuple<
		orm::column_meta_t<TestCaseF_Q_delete_TestModel, int>,
		orm::column_meta_t<TestCaseF_Q_delete_TestModel, std::string>
	> meta_columns;
};

const std::tuple<
	orm::column_meta_t<TestCaseF_Q_delete_TestModel, int>,
	orm::column_meta_t<TestCaseF_Q_delete_TestModel, std::string>
> TestCaseF_Q_delete_TestModel::meta_columns = {
	orm::make_column_meta("id", &TestCaseF_Q_delete_TestModel::id),
	orm::make_column_meta("name", &TestCaseF_Q_delete_TestModel::name)
};

class TestCaseF_Q_delete : public ::testing::Test
{
protected:
	orm::q::delete_<int, TestCaseF_Q_delete_TestModel>* query;

	void SetUp() override
	{
		this->query = new orm::q::delete_(
			&TestCaseF_Q_delete_TestModel::id,
			TestCaseF_Q_delete_TestModel()
		);
	}

	void TearDown() override
	{
		delete this->query;
	}
};

TEST_F(TestCaseF_Q_delete, exec_MissingDriverException)
{
	ASSERT_THROW(this->query->exec(), orm::QueryError);
}

TEST_F(TestCaseF_Q_delete, query_MissingDriverException)
{
	ASSERT_THROW(auto _ = this->query->query(), orm::QueryError);
}

TEST_F(TestCaseF_Q_delete, model_NoThrow)
{
	ASSERT_NO_THROW(this->query->model(TestCaseF_Q_delete_TestModel()));
}

TEST_F(TestCaseF_Q_delete, model_ThrowsNullModel)
{
	TestCaseF_Q_delete_TestModel null_model;
	null_model.mark_as_null();
	ASSERT_THROW(this->query->model(null_model), orm::QueryError);
}

TEST_F(TestCaseF_Q_delete, where_ThrowsCalledMoreThanOnce)
{
	ASSERT_THROW(
		this->query->where(orm::q::c(&TestCaseF_Q_delete_TestModel::id) == 1)
			.where(orm::q::like(&TestCaseF_Q_delete_TestModel::name, "%hn")),
		orm::QueryError
	);
}

TEST(TestCase_Q_delete, constructor_ThrowsNullModel)
{
	TestCaseF_Q_delete_TestModel null_model;
	null_model.mark_as_null();
	ASSERT_THROW(
		auto _ = orm::q::delete_(&TestCaseF_Q_delete_TestModel::id, null_model),
		orm::QueryError
	);
}
