/**
 * queries/tests_delete.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/delete.h"

using namespace xw;

struct TestModel : public orm::Model
{
	int id{};

	static constexpr const char* meta_table_name = "test_models";

	TestModel()
	{
		this->__attrs__ = {
			{"id", orm::field_accessor(&this->id)}
		};
	}

	TestModel(const TestModel& other) : TestModel()
	{
		if (this != &other)
		{
			this->id = other.id;
		}
	}
};

class TestCaseF_Q_delete : public ::testing::Test
{
protected:
	orm::q::delete_<TestModel>* query;

	void SetUp() override
	{
		this->query = new orm::q::delete_(TestModel());
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
	ASSERT_NO_THROW(this->query->model(TestModel()));
}

TEST_F(TestCaseF_Q_delete, model_ThrowsNullModel)
{
	TestModel null_model;
	null_model.mark_as_null();
	ASSERT_THROW(this->query->model(null_model), orm::QueryError);
}

TEST_F(TestCaseF_Q_delete, where_ThrowsCalledMoreThanOnce)
{
	ASSERT_THROW(
		this->query->where(orm::q::c<TestModel>("id") == 1)
			.where(orm::q::like<TestModel>("name", "%hn")),
		orm::QueryError
	);
}

TEST(TestCase_Q_delete, constructor_ThrowsNullModel)
{
	TestModel null_model;
	null_model.mark_as_null();
	ASSERT_THROW(
		auto _ = orm::q::delete_<TestModel>(null_model),
		orm::QueryError
	);
}
