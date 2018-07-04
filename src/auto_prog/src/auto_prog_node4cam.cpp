#include "ros/ros.h"
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>

#include "auto_prog/Gen_Cmd.h"
#include "cam_data/Yaw.h"
#include "reciever/Cmd.h"

#include <sstream>
#include <string>
#include <iostream>


// this is autonomous program that generates control commands and publishes to /command topic via auto_prog::Gen_Cmd message
// subscribes to /cam_yaw and publishes to /command

using namespace std;

class SubscribeAndPublish
{
public:
  SubscribeAndPublish()
  {
    read_pub = nh.advertise<auto_prog::Gen_Cmd>("/command", 1000);
    write_sub = nh.subscribe("/rec_data", 1000, &SubscribeAndPublish::rec_callback, this);
    cam_sub = nh.subscribe("/cam_yaw", 1000, &SubscribeAndPublish::cam_callback, this);
    rec_updated = false; cam_updated = false;
  }

  void rec_callback(const reciever::Cmd::ConstPtr& rec_msg) 
  {
    //.... do something with the input and generate the output...
    ROS_INFO_STREAM("Reading from /rec_data");
    rec_given_mode = rec_msg->mode;	
    rec_given_servo = rec_msg->servo;
    rec_given_vel = rec_msg->vel;
    rec_updated = true;

    if(cam_updated && rec_updated)
      Calc_and_Pub();
  }

  void cam_callback(const cam_data::Yaw::ConstPtr& cam_msg) 
  {
    //.... do something with the input and generate the output...
    ROS_INFO_STREAM("Reading from /cam_yaw");
    
    cam_given_yaw = cam_msg-> yaw_num;
    cam_updated = true;

    if(cam_updated && rec_updated)
      Calc_and_Pub();
  }

  void Calc_and_Pub()
  {
    auto_prog::Gen_Cmd msg;
    //msg.mode = rec_given_mode;	
    msg.mode = 1.0;
    //msg.vel = rec_given_vel;
    /*if (cam_given_yaw == -1)
    {
	msg.vel = 5; //previously kept this at 0 so that it would stop when lanes/lines weren't detected
    }
    else{
        msg.vel = 5;
    }*/
    msg.vel=5;
    msg.servo = cam_given_yaw;

    ROS_INFO_STREAM("Publishing to /command");
    read_pub.publish(msg);    
  }

private:
  ros::NodeHandle nh; 
  ros::Publisher read_pub;
  ros::Subscriber write_sub;
  ros::Subscriber cam_sub;
  float cam_given_yaw, rec_given_mode, rec_given_servo, rec_given_vel;
  bool rec_updated, cam_updated;

};//End of class SubscribeAndPublish


int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "subscribe_and_publish");

  //Create an object of class SubscribeAndPublish that will take care of everythin
  SubscribeAndPublish SAPObject;

  ros::spin();

  return 0;
}
