/**
 * queries/tests_select.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/select.h"

using namespace xw;

struct TestModel : public orm::Model
{
};

class TestCase_Q_select : public ::testing::Test
{
protected:
	orm::q::select<TestModel>* query;

	void SetUp() override
	{
		this->query = new orm::q::select<TestModel>();
	}

	void TearDown() override
	{
		delete this->query;
	}
};

TEST_F(TestCase_Q_select, distinct_NotThrow)
{
	ASSERT_NO_THROW(this->query->distinct());
}

TEST_F(TestCase_Q_select, where_NotThrow)
{
	ASSERT_NO_THROW(this->query->where(orm::q::eq("id", 1)));
}

TEST_F(TestCase_Q_select, order_by_NotThrow)
{
	ASSERT_NO_THROW(this->query->order_by({orm::q::asc("id")}));
}

TEST_F(TestCase_Q_select, order_by_MultipleCalls_NotThrow_WhenFirstCallArgsIsEmpty)
{
	ASSERT_NO_THROW(this->query->order_by({}).order_by({orm::q::asc("id")}));
}

TEST_F(TestCase_Q_select, limit_NotThrow)
{
	ASSERT_NO_THROW(this->query->limit(1));
}

TEST_F(TestCase_Q_select, offset_NotThrow)
{
	ASSERT_NO_THROW(this->query->offset(1));
}

TEST_F(TestCase_Q_select, offset_MultipleCalls_NotThrow_WhenPreviousCallsArgsIsEmpty)
{
	ASSERT_NO_THROW(this->query->offset(0).offset(1));
}

TEST_F(TestCase_Q_select, group_by_NotThrow)
{
	ASSERT_NO_THROW(this->query->group_by({"id"}));
}

TEST_F(TestCase_Q_select, group_by_MultipleCalls_NotThrow_WhenFirstCallArgsIsEmpty)
{
	ASSERT_NO_THROW(this->query->group_by({}).group_by({"id"}));
}

TEST_F(TestCase_Q_select, having_NotThrow)
{
	ASSERT_NO_THROW(this->query->having(orm::q::eq("id", 1)));
}

TEST_F(TestCase_Q_select, first_ThrowsClientNotSet)
{
	ASSERT_THROW(this->query->first(), orm::QueryError);
}

TEST_F(TestCase_Q_select, to_vector_ThrowsClientNotSet)
{
	ASSERT_THROW(this->query->to_vector(), orm::QueryError);
}

TEST_F(TestCase_Q_select, query_ThrowsClientNotSet)
{
	ASSERT_THROW(auto _ = this->query->query(), orm::QueryError);
}

TEST_F(TestCase_Q_select, distinct_Throws_MultipleCallsException)
{
	ASSERT_THROW(this->query->distinct().distinct(), orm::QueryError);
}

TEST_F(TestCase_Q_select, where_Throws_MultipleCallsException)
{
	ASSERT_THROW(
		this->query->where(orm::q::eq("id", 1)).where(orm::q::eq("name", "John")),
		orm::QueryError
	);
}

TEST_F(TestCase_Q_select, order_by_Throws_MultipleCallsException)
{
	ASSERT_THROW(
		this->query->order_by({orm::q::asc("id")}).order_by({orm::q::desc("name")}),
		orm::QueryError
	);
}

TEST_F(TestCase_Q_select, limit_Throws_MultipleCallsException)
{
	ASSERT_THROW(this->query->limit(1).limit(2), orm::QueryError);
}

TEST_F(TestCase_Q_select, offset_Throws_MultipleCallsException)
{
	ASSERT_THROW(this->query->offset(1).offset(2), orm::QueryError);
}

TEST_F(TestCase_Q_select, group_by_Throws_MultipleCallsException)
{
	ASSERT_THROW(this->query->group_by({"id"}).group_by({"name"}), orm::QueryError);
}

TEST_F(TestCase_Q_select, having_Throws_MultipleCallsException)
{
	ASSERT_THROW(
		this->query->having(orm::q::eq("id", 1)).having(orm::q::eq("name", "John")),
		orm::QueryError
	);
}
