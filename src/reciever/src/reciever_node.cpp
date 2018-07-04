#include "ros/ros.h"
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include "reciever/Cmd.h"
#include <sstream>
#include <string>
#include "serial/serial.h"

// have to publish Cmd from serial to topic /rec_data

using namespace std;

serial::Serial ser;

int main (int argc, char** argv){
	ros::init(argc, argv, "serial_example_node");
	ros::NodeHandle nh;

	ros::Publisher read_pub = nh.advertise<reciever::Cmd>("/rec_data", 1000);
	//ros::Publisher read_pub = nh.advertise<std_msgs::String>("/rec_data", 1000);
	try
	{
		ser.setPort("/dev/ttyACM1");
		ser.setBaudrate(115200);

		serial::Timeout to = serial::Timeout::simpleTimeout(1000);
		ser.setTimeout(to);

		ser.open();
	}
	catch (serial::IOException& e)
	{
		ROS_ERROR_STREAM("Unable to open port ");
		return -1;
	}
	if(ser.isOpen()){
		ROS_INFO_STREAM("Serial Port initialized");
	}else{
		return -1;
	}

	//ros::Rate loop_rate(10);
	while(ros::ok()){
		//ros::spinOnce();
		if(ser.available()){
			ROS_INFO_STREAM("Reading from serial port");

			reciever::Cmd msg;
			string line_read = ser.readline();
			
			if (count(line_read.begin() ,line_read.end(), ',') == 2){
				char const *line = ser.readline().c_str();
				cout<< line<<endl;
			
				//parsing (only if 2 cdommas present)
				char *ssl1,*ssl2,*ssl3; //ssl = stop string location
				char const *ss1 = strstr(line,",");
				char const *ss2 = strstr(ss1+1,","); // ss1 = split string 1 			

				msg.mode = strtoul(line,&ssl1,10);
				msg.servo = strtoul(ss1+1,&ssl2,10);
				msg.vel = strtoul(ss2+1,&ssl3,10);

				//ROS_INFO_STREAM("Read: " << result.data);
				read_pub.publish(msg);
			}
		}
		ros::spinOnce();
		//loop_rate.sleep();
	}
}

