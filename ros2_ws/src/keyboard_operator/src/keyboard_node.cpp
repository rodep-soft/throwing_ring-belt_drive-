#include <cstdio>
#include <iostream>
#include <termios.h>
#include <unistd.h>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

//Nodeを継承したクラスの作成
class KeyboardNode : public rclcpp::Node {
	public:
		//constructor: 
		//node name: keyboard_node
		KeyboardNode() : Node("keyboard_node"){
			tcgetattr(STDIN_FILENO, &old_settings_);
			//topic name
			publisher_ = this->create_publisher<std_msgs::msg::String>("key_input",10);

			RCLCPP_INFO(this->get_logger(),"Publisher created on /key_input");
		}
		~KeyboardNode(){
			tcsetattr(STDIN_FILENO, TCSANOW, &old_settings_);
		}

		void run() {
			RCLCPP_INFO(this->get_logger(),"Running keyboard loop...");
			while (rclcpp::ok()){
				char key = get_key();
				publish_key(key);
				if(key == 27) break;
			}
		}

	private:
		//元のターミナルの設定を格納する構造体
		struct termios old_settings_;

		// publisher (using Publisher in rclcpp)
		rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
		
		char get_key(){
			//これから使うターミナルの設定を格納する構造体
			struct termios new_settings = old_settings_;
			//turn off canonical mode and echo mode
			new_settings.c_lflag &= ~(ICANON | ECHO);
			//新しい設定をターミナルに反映
			tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);

			char c;
			// 標準入力から1バイト（1文字）読み取る。入力があるまでここで止まる。
			read(STDIN_FILENO, &c, 1);

			tcsetattr(STDIN_FILENO, TCSANOW, &old_settings_);

			return c;
			}

		void publish_key(char key) {
			auto message = std_msgs::msg::String();
			message.data = std::string(1,key);
			publisher_ -> publish(message);
		}
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  //creating an instance
  auto node = std::make_shared<KeyboardNode>();

  //rclcpp::spin(node);
  node->run();
  rclcpp::shutdown();

  return 0;
}
