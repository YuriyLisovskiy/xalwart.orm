/**
 * queries/tests_select.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include "../../src/queries/select.h"

using namespace xw;

struct TestCase_Q_TestModel : public orm::Model<TestCase_Q_TestModel>
{
	static constexpr const char* meta_table_name = "test_model";

	int id{};
	std::string name;

	static const std::tuple<
		orm::column_meta_t<TestCase_Q_TestModel, int>,
		orm::column_meta_t<TestCase_Q_TestModel, std::string>
	> meta_columns;
};

const std::tuple<
	orm::column_meta_t<TestCase_Q_TestModel, int>,
	orm::column_meta_t<TestCase_Q_TestModel, std::string>
> TestCase_Q_TestModel::meta_columns = {
	orm::make_column_meta("id", &TestCase_Q_TestModel::id),
	orm::make_column_meta("name", &TestCase_Q_TestModel::name)
};

class TestCase_Q_select : public ::testing::Test
{
protected:
	orm::q::select<TestCase_Q_TestModel>* query;

	void SetUp() override
	{
		this->query = new orm::q::select<TestCase_Q_TestModel>();
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
	ASSERT_NO_THROW(this->query->where(orm::q::c(&TestCase_Q_TestModel::id) == 1));
}

TEST_F(TestCase_Q_select, order_by_NotThrow)
{
	ASSERT_NO_THROW(this->query->order_by({orm::q::asc(&TestCase_Q_TestModel::id)}));
}

TEST_F(TestCase_Q_select, order_by_MultipleCalls_NotThrow_WhenFirstCallArgsIsEmpty)
{
	ASSERT_NO_THROW(this->query->order_by({}).order_by({orm::q::asc(&TestCase_Q_TestModel::id)}));
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
	ASSERT_NO_THROW(this->query->having(orm::q::c(&TestCase_Q_TestModel::id) == 1));
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
	ASSERT_THROW(this->query->where(orm::q::c(&TestCase_Q_TestModel::id) == 1)
		.where(orm::q::c(&TestCase_Q_TestModel::name) == "John"), orm::QueryError);
}

TEST_F(TestCase_Q_select, order_by_Throws_MultipleCallsException)
{
	ASSERT_THROW(
		this->query->order_by({orm::q::asc(&TestCase_Q_TestModel::id)})
			.order_by({orm::q::desc(&TestCase_Q_TestModel::name)}),
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
	ASSERT_THROW(
		this->query->group_by({"id"}).group_by({"name"}), orm::QueryError
	);
}

TEST_F(TestCase_Q_select, having_Throws_MultipleCallsException)
{
	ASSERT_THROW(this->query->having(orm::q::c(&TestCase_Q_TestModel::id) == 1)
		.having(orm::q::c(&TestCase_Q_TestModel::name) == "John"), orm::QueryError);
}
