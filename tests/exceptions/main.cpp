/**
 * exceptions/main.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
