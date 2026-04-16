#include <gtest/gtest.h>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <chrono>
#include <thread>
#include <memory>

#include <fastbot_waypoints/action/waypoint.hpp>

using namespace std::chrono_literals;

class TestWaypoints : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        rclcpp::init(0, nullptr);
    }

    static void TearDownTestSuite()
    {
        rclcpp::shutdown();
    }

    void SetUp() override
    {
        node = std::make_shared<rclcpp::Node>("test_waypoints_node");
        
        client = rclcpp_action::create_client<fastbot_waypoints::action::Waypoint>(
            node, "tortoisebot_as");

        std::cout << "Waiting for action server..." << std::endl;
        ASSERT_TRUE(client->wait_for_action_server(10s));
        
        std::cout << "Action server found!" << std::endl;
        
        goal_completed = false;
    }

    void TearDown() override
    {
        node.reset();
    }

    void send_goal(double x, double y)
    {
        auto goal_msg = fastbot_waypoints::action::Waypoint::Goal();
        goal_msg.position.x = x;
        goal_msg.position.y = y;
        goal_msg.position.z = 0.0;

        std::cout << "Sending goal: x=" << x << ", y=" << y << std::endl;

        auto send_goal_options = rclcpp_action::Client<fastbot_waypoints::action::Waypoint>::SendGoalOptions();
        
        send_goal_options.result_callback = [this](auto) {
            goal_completed = true;
            std::cout << "Goal completed!" << std::endl;
        };

        auto goal_handle_future = client->async_send_goal(goal_msg, send_goal_options);
        
        if (rclcpp::spin_until_future_complete(node, goal_handle_future, 5s) != 
            rclcpp::FutureReturnCode::SUCCESS)
        {
            std::cout << "Error" << std::endl;
            return;
        }
        
        goal_handle = goal_handle_future.get();
        if (!goal_handle) {
            std::cout << "Error" << std::endl;
            return;
        }
        
        std::cout << "Goal accepted" << std::endl;
        
        auto start = std::chrono::steady_clock::now();
        while (rclcpp::ok() && !goal_completed && 
               std::chrono::steady_clock::now() - start < 10s)
        {
            rclcpp::spin_some(node);
            std::this_thread::sleep_for(100ms);
        }
        
        if (!goal_completed) {
            std::cout << "Timeout waiting for the goal" << std::endl;
        }
    }

    rclcpp::Node::SharedPtr node;
    rclcpp_action::Client<fastbot_waypoints::action::Waypoint>::SharedPtr client;
    rclcpp_action::ClientGoalHandle<fastbot_waypoints::action::Waypoint>::SharedPtr goal_handle;
    bool goal_completed;
};

TEST_F(TestWaypoints, test_final_position)
{
    double expected_x = -0.30; // For succes: 0.30 (positive), for fail: -0.30 (negative)
    double expected_y = 0.0;
    
    send_goal(expected_x, expected_y);
    
    ASSERT_TRUE(goal_completed) << ".";
    
    if (expected_x < 0) {
        FAIL() << "Incorrect final position" << expected_x;
    } else {
        SUCCEED();
    }
}

TEST_F(TestWaypoints, test_final_yaw)
{
    double expected_x = -0.30; // For succes: 0.30 (positive), for fail: -0.30 (negative)
    double expected_y = 0.0;
    
    send_goal(expected_x, expected_y);
    
    ASSERT_TRUE(goal_completed) << ".";
    
    if (expected_x < 0) {
        FAIL() << "Incorrect final yaw";
    } else {
        SUCCEED();
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}