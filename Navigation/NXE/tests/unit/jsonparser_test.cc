#include <iostream>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>
#include <algorithm>
#include <cctype>

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include "jsonmessage.h"

class JSONParserTest : public ::testing::Test
{
};

TEST_F(JSONParserTest, invalid_json_only_id)
{
    BOOST_LOG_TRIVIAL(info) << "invalid";
    const std::string incomingMessage = "{\"id\":0}";
    EXPECT_ANY_THROW(
        const NXE::JSONMessage message = NXE::JSONUtils::deserialize(incomingMessage)
    );
}
TEST_F(JSONParserTest, minimal_proper_json)
{
    const std::string incomingMessage = "{\"id\":0, \"call\":\"pan\"}";
    EXPECT_NO_THROW(
        NXE::JSONMessage msg = std::move( NXE::JSONUtils::deserialize(incomingMessage))
    );
}

TEST_F(JSONParserTest, minimal_proper_json_with_values)
{
    const std::string incomingMessage = "{\"id\":0, \"call\":\"pan\"}";
    NXE::JSONMessage msg = std::move( NXE::JSONUtils::deserialize(incomingMessage));

    EXPECT_EQ(msg.id, 0);
    EXPECT_EQ(msg.call, "pan");
}

TEST_F(JSONParserTest, full_json_message)
{
    const std::string incomingMessage = "{\"id\":0, \"call\":\"pan\", \"data\":\"some data\"}";
    NXE::JSONMessage msg = std::move( NXE::JSONUtils::deserialize(incomingMessage));

    EXPECT_EQ(msg.id, 0);
    EXPECT_EQ(msg.call, "pan");
    EXPECT_EQ(msg.data.value(), "some data");
}

// No need to create invalid test case as it's impossible to create invalid JSONMessage
TEST_F(JSONParserTest, DISABLED_serialize_invalid)
{
    NXE::JSONMessage invalidMsg {0,""};
}

TEST_F(JSONParserTest, serialize_valid)
{
    NXE::JSONMessage msg {105, "pan"};
    std::string data = NXE::JSONUtils::serialize(msg);

    boost::erase_all(data, " ");
    boost::erase_all(data, "\n");

    const std::string expected = "{\"id\":\"105\",\"call\":\"pan\"}";

    EXPECT_EQ(data, expected);
}
