#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <std_srvs/srv/empty.hpp>

class LauncherNode : public rclcpp::Node {
	public:
		LauncherNode() : Node("launcher_node"){
			server_ = this->create_service<std_srvs::srv::Empty>(
		"start_launch", std::bind(&LauncherNode::handle_service, this, std::placeholders::_1, std::placeholders::_2));
		}

		void run(){
			start_subscribe_scan();
		}

	private:
		int height = 0.70;
		bool is_shooting = false;

		rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscriber_;
		rclcpp::Service<std_srvs::srv::Empty>::SharedPtr server_;
		//record distance data
		float distance_;//---------------------------
		
		void start_subscribe_scan() {
			subscriber_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
				"scan",5,std::bind(&LauncherNode::scan_callback, this, std::placeholders::_1));
		}	
		//callback function which save the data in distance_ and calculate average
		void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
			//----------
			RCLCPP_INFO(this->get_logger(), "distance middle %f ", msg->ranges[540]);
			RCLCPP_INFO(this->get_logger(), "num of data %zu", msg->ranges.size());
		}
		void handle_service(const std::shared_ptr<std_srvs::srv::Empty::Request> request,
				std::shared_ptr<std_srvs::srv::Empty::Response> response)
		{
			(void)request;
			(void)response;

			is_shooting = true;

		//----------calculate goal position()
		}
		float calculate_goal_position(float distance, float height){
		//-------------------------
		}
};

int main (int argc, char **argv) {
	rclcpp::init(argc, argv);
	
	auto node = std::make_shared<LauncherNode>();
	node->run();
	rclcpp::shutdown();
	return 0;
}
