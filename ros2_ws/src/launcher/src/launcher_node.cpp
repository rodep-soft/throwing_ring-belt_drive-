#include <iostream>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <std_srvs/srv/empty.hpp>
#include <vector>
#include <deque>
#include <cmath>

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
		float target_height_ = 0.70;
		float m = 0.43;  // mass
		float g_ = 9.8;  // gravitational acceleraition
		float drag_coefficient ;
		float density;
		float area;
		float k = drag_coefficient * density * area / 2;
		float gamma_ = k / m;
		float angle = M_PI / 4;
		float tan_theta_ = std::tan(angle);
		float cos_theta_ = std::cos(angle);
		float target_distance_ = 0;
		bool is_shooting = false;

		rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscriber_;
		rclcpp::Service<std_srvs::srv::Empty>::SharedPtr server_;
		//record distance data
		std::deque<sensor_msgs::msg::LaserScan::ConstSharedPtr> scan_buffer;
		
		void start_subscribe_scan() {
			subscriber_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
				"scan",5,std::bind(&LauncherNode::scan_callback, this, std::placeholders::_1));
		}	
		//callback function which save the data in distance_ and calculate average
		void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
			//----------
			RCLCPP_INFO(this->get_logger(), "distance middle %f ", msg->ranges[540]);
			RCLCPP_INFO(this->get_logger(), "num of data %zu", msg->ranges.size());

			target_distance_ = calculate_distance_average(msg);
			//queueにいれる();

		}

		float calculate_distance_average(const sensor_msgs::msg::LaserScan::SharedPtr msg){
			int n = 10;
			float sum_distance = 0;
			int count = 0;
			for(int i=0; i<n;i++){
				float distance = msg->ranges[(msg->ranges.size()-n)/2+i+1];
				if(distance > 0 && distance < 40){
					sum_distance += distance;
					count ++;
				}
			}
			
			if (count == 0) return 0.0;

			return sum_distance / count;
		}

		void handle_service(const std::shared_ptr<std_srvs::srv::Empty::Request> request,
				std::shared_ptr<std_srvs::srv::Empty::Response> response)
		{
			(void)request;
			(void)response;

			is_shooting = true;
			float velocity = solve_velocity(target_distance_, target_height_);
			
			//shoot(velocity);
		}
		float solve_velocity(float dist, float height){
			float v = 8.0;
			const float epsilon = 1e-3;
			const int max_iter = 100;
			int iter = 0;
			
			float y_error = calc_y_error(v, dist, height);  
			float dy_dv = calc_dy_dv(v, dist);
			while(std::abs(y_error) > epsilon && iter < max_iter) {
				

				if (dy_dv < 1e-6) break;

				v = v - y_error / dy_dv;
				iter ++;

				y_error = calc_y_error(v, dist, height);  
				dy_dv = calc_dy_dv(v, dist);
			}
			

			return v;
		}

		float calc_y_error(float v0, float x, float target_y){
			float y = (tan_theta_ + g_ /gamma_ /v0 /cos_theta_)*x +g_ /gamma_/gamma_ *std::log(1 -gamma_ *x /v0 /cos_theta_);
			return y - target_y;
		}

		float calc_dy_dv(float v0, float x) {
			float dy_dv = g_ *x*x / (v0*v0*cos_theta_ *(v0 *cos_theta_ -gamma_ *x));
			return dy_dv;
		}
};

int main (int argc, char **argv) {
	rclcpp::init(argc, argv);
	
	auto node = std::make_shared<LauncherNode>();
	node->run();
	rclcpp::shutdown();
	return 0;
}
