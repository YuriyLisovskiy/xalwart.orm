/**
 * queries/tests_insert.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/insert.h"

using namespace xw;

struct TestModel : public orm::Model
{
};

class TestCase_Q_insert_One : public ::testing::Test
{
protected:
	orm::q::insert<TestModel>* query;

	void SetUp() override
	{
		this->query = new orm::q::insert(TestModel());
	}

	void TearDown() override
	{
		delete this->query;
	}
};

TEST_F(TestCase_Q_insert_One, one_MissingDriverException)
{
	ASSERT_THROW(auto _ = this->query->one(), orm::QueryError);
}

TEST_F(TestCase_Q_insert_One, one_WithPkArg_MissingDriverException)
{
	int pk;
	ASSERT_THROW(this->query->one(pk), orm::QueryError);
}

TEST_F(TestCase_Q_insert_One, bulk_MissingDriverException)
{
	ASSERT_THROW(this->query->bulk(), orm::QueryError);
}

TEST_F(TestCase_Q_insert_One, query_MissingDriverException)
{
	ASSERT_THROW(auto _ = this->query->query(), orm::QueryError);
}

class TestCase_Q_insert_Bulk : public ::testing::Test
{
protected:
	orm::q::insert<TestModel>* query;

	void SetUp() override
	{
		this->query = new orm::q::insert(TestModel());
		this->query->model(TestModel());
	}

	void TearDown() override
	{
		delete this->query;
	}
};

TEST_F(TestCase_Q_insert_Bulk, one_FailDueToBulkMode)
{
	ASSERT_THROW(auto _ = this->query->one(), orm::QueryError);
}

TEST_F(TestCase_Q_insert_One, one_WithPkArg_FailDueToBulkMode)
{
	int pk;
	ASSERT_THROW(this->query->one(pk), orm::QueryError);
}
