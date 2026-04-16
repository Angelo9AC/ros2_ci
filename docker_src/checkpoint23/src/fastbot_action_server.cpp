#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <chrono>
#include <memory>
#include <cmath>

#include "fastbot_waypoints/action/waypoint.hpp"

using namespace std::chrono_literals;

class WaypointActionServer : public rclcpp::Node
{
public:
    using Waypoint = fastbot_waypoints::action::Waypoint;
    using GoalHandleWaypoint = rclcpp_action::ServerGoalHandle<Waypoint>;

    WaypointActionServer() : Node("fastbot_action_server")
    {
        move_duration_ = 3.0;
        base_speed_ = 0.1;

        action_server_ = rclcpp_action::create_server<Waypoint>(
            this,
            "tortoisebot_as",
            std::bind(&WaypointActionServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&WaypointActionServer::handle_cancel, this, std::placeholders::_1),
            std::bind(&WaypointActionServer::handle_accepted, this, std::placeholders::_1));

        publishers_.push_back(this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10));
        publishers_.push_back(this->create_publisher<geometry_msgs::msg::Twist>("/fastbot/cmd_vel", 10));
        publishers_.push_back(this->create_publisher<geometry_msgs::msg::Twist>("/mobile_base/cmd_vel", 10));
        publishers_.push_back(this->create_publisher<geometry_msgs::msg::Twist>("/diff_drive_controller/cmd_vel", 10));
        publishers_.push_back(this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel_mux/input/teleop", 10));

        RCLCPP_INFO(this->get_logger(), "Starting action server");
    }

private:
    rclcpp_action::Server<Waypoint>::SharedPtr action_server_;
    std::vector<rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr> publishers_;

    double move_duration_;
    double base_speed_;
    double current_goal_x_;

    void publish_twist(const geometry_msgs::msg::Twist & twist)
    {
        for (auto & pub : publishers_) {
            pub->publish(twist);
        }
    }

    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const Waypoint::Goal> goal)
    {
        (void)uuid;
        current_goal_x_ = goal->position.x;
        
        RCLCPP_INFO(this->get_logger(), "Goal received: (%.2f, %.2f)", 
                    goal->position.x, goal->position.y);
        
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandleWaypoint> goal_handle)
    {
        (void)goal_handle;
        RCLCPP_INFO(this->get_logger(), "Goal cancelled");
        
        geometry_msgs::msg::Twist stop;
        stop.linear.x = 0.0;
        stop.angular.z = 0.0;
        publish_twist(stop);
        
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handle_accepted(const std::shared_ptr<GoalHandleWaypoint> goal_handle)
    {
        std::thread{std::bind(&WaypointActionServer::execute, this, std::placeholders::_1), goal_handle}.detach();
    }

    void execute(const std::shared_ptr<GoalHandleWaypoint> goal_handle)
    {
        const auto goal = goal_handle->get_goal();
        double speed = base_speed_;
        std::string direction;
        
        if (goal->position.x > 0) {
            speed = base_speed_;  // Forward
            direction = "Forward";
        } else if (goal->position.x < 0) {
            speed = -base_speed_;  // Backward
            direction = "Backward";
        } else {
            speed = 0.0;  // Quiet
            direction = "Quiet";
        }
        
        auto feedback = std::make_shared<Waypoint::Feedback>();
        auto result = std::make_shared<Waypoint::Result>();
        
        geometry_msgs::msg::Twist twist;
        twist.linear.x = speed;
        twist.angular.z = 0.0;
        
        RCLCPP_INFO(this->get_logger(), "Publishing cmd_vel: linear=%.2f", 
                    speed);
        
        auto start_time = this->now();
        rclcpp::Rate rate(20);
        
        int count = 0;
        while (rclcpp::ok())
        {
            auto elapsed = (this->now() - start_time).seconds();
            
            if (elapsed > move_duration_)
            {
                RCLCPP_INFO(this->get_logger(), "Time completed: %.1f seconds", elapsed);
                break;
            }
            
            if (goal_handle->is_canceling())
            {
                RCLCPP_INFO(this->get_logger(), "Goal cancelled during trajectory");
                twist.linear.x = 0.0;
                publish_twist(twist);
                
                result->success = false;
                goal_handle->canceled(result);
                return;
            }
            
            publish_twist(twist);
            
            count++;
            if (count % 50 == 0) {
                RCLCPP_INFO(this->get_logger(), "Publishing cmd_vel... (%.1f seg)", elapsed);
            }
            
            feedback->state = "moving " + direction;
            goal_handle->publish_feedback(feedback);
            
            rate.sleep();
        }
        
        twist.linear.x = 0.0;
        publish_twist(twist);
        
        result->success = true;
        goal_handle->succeed(result);
        RCLCPP_INFO(this->get_logger(), "Goal completed");
    }
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<WaypointActionServer>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}