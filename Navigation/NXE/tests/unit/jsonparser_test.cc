#include <iostream>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <algorithm>
#include <cctype>

#include <boost/algorithm/string.hpp>
#include "jsonmessage.h"

struct JSONParserTest : public ::testing::Test {
};

TEST_F(JSONParserTest, invalid_json_only_id)
{
    const std::string incomingMessage = "{\"id\":0}";
    EXPECT_ANY_THROW(
        const NXE::JSONMessage message = NXE::JSONUtils::deserialize(incomingMessage));
}

TEST_F(JSONParserTest, moveBy_message_with_args)
{
}

// parsing shouldn't raise an exception, only handling should
TEST_F(JSONParserTest, moveBy_message_without_args)
{
    const std::string incomingMessage = "{\"id\":0, \"call\":\"moveBy\"}";
    EXPECT_ANY_THROW(
    NXE::JSONMessage msg = std::move(NXE::JSONUtils::deserialize(incomingMessage));

    EXPECT_EQ(msg.id, 0);
    EXPECT_EQ(msg.call, "moveBy");
    EXPECT_FALSE(msg.data.empty());
    );
}

TEST_F(JSONParserTest, full_json_message)
{
    const std::string incomingMessage = "{\"id\":0, \"call\":\"moveBy\", \"data\":{}}";
    EXPECT_NO_THROW(
    NXE::JSONMessage msg = std::move(NXE::JSONUtils::deserialize(incomingMessage));

    EXPECT_EQ(msg.id, 0);
    EXPECT_EQ(msg.call, "moveBy");
    EXPECT_TRUE(msg.data.empty());
    );
}

// No need to create invalid test case as it's impossible to create invalid JSONMessage
TEST_F(JSONParserTest, DISABLED_serialize_invalid)
{
    NXE::JSONMessage invalidMsg{ 0, "" };
}

TEST_F(JSONParserTest, serialize_valid)
{
    NXE::JSONMessage msg{ 105, "moveBy" };
    std::string data = NXE::JSONUtils::serialize(msg);

    boost::erase_all(data, " ");
    boost::erase_all(data, "\n");

    const std::string expected = "{\"id\":\"105\",\"call\":\"moveBy\"}";

    EXPECT_EQ(data, expected);
}
