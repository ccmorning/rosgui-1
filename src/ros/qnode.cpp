/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include "../../include/rosgui/ros/qnode.hpp"
#include <tf/tf.h>
#include "t3_description/goal.h"
#include "yaml-cpp/yaml.h"
#include <math.h>
#include <std_msgs/Bool.h>
#include <tf/transform_listener.h>

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace rosgui {

/*****************************************************************************
** Implementation
*****************************************************************************/

QNode * QNode::getInstance()
{
  static QNode qnode;
  return &qnode;
}

QNode::QNode():
  _mapOrigin(3, 0.0),
  _robotPose(4, 0.0),
  _oprationMode(Manual),
  _linearX(0.0),
  _angularZ(0.0),
  _battPer(100),
  _OdomLinearX(0.0),
  _OdomAngularZ(0.0)
{
}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
//      qDebug() << "shutdown";
    }
	wait();
}

bool QNode::init(int argc, char** argv ) {
  init_argc = argc;
  init_argv = argv;

  ros::init(init_argc,init_argv,"rosgui");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
  ros::NodeHandle pn("~");
  getParam(pn);
  // pub
	chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
  _robotGoal = n.advertise<t3_description::goal>("robotGoal", 100);
  _cmdVelPub = n.advertise<geometry_msgs::Twist>("cmd_vel", 100);
  _checkNetPub = n.advertise<std_msgs::Bool>("checkNet",10);
  _oprationModePub = n.advertise<std_msgs::Bool>("oprationMode", 10);
  //sub
  _odomSub = n.subscribe("odom", 100, &QNode::getOdomCallback, this);
  _globalPlanSub = n.subscribe(_globalPlanTopicName.c_str(), 1000, &QNode::getGlobalPlanCallback, this);
  _batterySub = n.subscribe("sensor_state",100, &QNode::getStateCallback, this);

	start();
	return true;
}

//bool QNode::init(const std::string &master_url, const std::string &host_url) {
//	std::map<std::string,std::string> remappings;
//	remappings["__master"] = master_url;
//	remappings["__hostname"] = host_url;
//	ros::init(remappings,"rosgui");
//	if ( ! ros::master::check() ) {
//		return false;
//	}
//	ros::start(); // explicitly needed since our nodehandle is going out of scope.
//	ros::NodeHandle n;
//	// Add your ros communications here.
//	chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
//  getParam(n);
//	start();
//	return true;
//}

void QNode::getParam(ros::NodeHandle& n)
{
//  double tempX;
  //get param
//  n.param("robotPoseTopicName", _robotPoseTopicName, std::string("/odometry/filtered_map"));
  n.param("globalPlanTopicName", _globalPlanTopicName, std::string("/TrajectoryPlannerROS/global_plan"));
  n.param("originX", _mapOrigin[0], 0.0);
  n.param("originY", _mapOrigin[1], 0.0);
  n.param("originZ", _mapOrigin[2], 0.0);
  n.param("maxLinearX", _maxLinearX, 5.0);
  n.param("maxAngularZ", _maxAngularZ, 5.0);


//  ROS_WARN("set param");
//  T3LOG(_globalPlanTopicName.c_str());
//  std::count << "set param" << std::endl;
}

///
/// \brief QNode::shutDownRos
///
void QNode::shutDownRos()
{

//  if(ros::isStarted()) {
    ros::shutdown(); // explicitly needed since we use ros::start();
//    qDebug() << "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
    ros::waitForShutdown();
//  }
}

//**********************call back********************//
void QNode::getStateCallback(const SensorState &msg)
{
  _battPer = (int)msg.battery;
  if(_battPer < kLowPowerLimit)
  {
//    Q_EMIT lowPower();
  }
}

void QNode::getOdomCallback(const nav_msgs::Odometry& msg)
{
//  if(msg.header.frame_id == "")
//  {
//    ROS_WARN("Received initial pose with empty frame_id.  You should always supply a frame_id.");
//  }
  //ROS_WARN("get pose");
//  _robotPose[0] = msg.pose.pose.position.x;
//  _robotPose[1] = msg.pose.pose.position.y;
//  _robotPose[2] = msg.pose.pose.orientation.z;
//  _robotPose[3] = msg.pose.pose.orientation.w;

  _OdomLinearX = msg.twist.twist.linear.x;
  _OdomAngularZ = msg.twist.twist.angular.z;
//  Q_EMIT poseUpdated();
}

///
/// \brief getGlobalPlanCallback
/// \param pathMsg
///
void QNode::getGlobalPlanCallback(const nav_msgs::Path& pathMsg)
{
  if(pathMsg.header.frame_id == "")
    {
      // This should be removed at some point
      ROS_WARN("Received path with empty frame_id.  You should always supply a frame_id.");
    }
  _globalPlan.clear();
  int pathSize = pathMsg.poses.size();
  for(unsigned int i=0; i < pathSize; i++)
  {
    geometry_msgs::PoseStamped pose = pathMsg.poses[i];
    _globalPlan.append(std::make_pair(pose.pose.position.x, pose.pose.position.y));
  }

  Q_EMIT globalPlanGet();
}

void QNode::run() {
  tf::TransformListener listener;
  ros::Rate loop_rate(10);
//  int count = 0;

	while ( ros::ok() ) {
    tf::StampedTransform transform;
    try{
      ros::Time now = ros::Time::now();
      listener.waitForTransform("/map", "/base_footprint",
                                now, ros::Duration(3.0));
      listener.lookupTransform("/map", "/base_footprint",
                               now, transform);

      _robotPose[0] = transform.getOrigin().x();
      _robotPose[1] = transform.getOrigin().y();
      _robotPose[2] = transform.getRotation().getZ();
      _robotPose[3] = transform.getRotation().getW();

      Q_EMIT poseUpdated();
    }

    catch (tf::TransformException &ex) {
      ROS_ERROR("%s",ex.what());
      ros::Duration(1.0).sleep();
      continue;
    }
//    std_msgs::String msg;
//    std::stringstream ss;
//    ss << "hello world " << count;
//    msg.data = ss.str();
//    chatter_publisher.publish(msg);
//    log(Info,std::string("I sent: ")+msg.data);
//    std_msgs::Bool checkMsg;
//    checkMsg.data = true;
//    _checkNetPub.publish(checkMsg);
		ros::spinOnce();
		loop_rate.sleep();
//		++count;
	}
//	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}

///
/// \brief when goal update, publish it.
/// \param pose x
/// \param pose y
/// \param pose_oritation z
///
bool QNode::goalUpdate(float x, float y, float z)
{
  if(_oprationMode == Auto)
  {
    t3_description::goal goalMsg_;
    goalMsg_.x = x;
    goalMsg_.y = y;
    goalMsg_.z = z;
    double z0 = sin(z/2);
    double w = cos(z/2);
    double angle = atan2(2*(w*z0), 1-2*z0*z0);
    ROS_INFO("%f %f", z, angle);
   // qDebug() << "get pose";
    _robotGoal.publish(goalMsg_);
    return true;
  }else
  {
    return false;
  }
}

///
/// \brief publish robot speed cmd
///
void QNode::pubRobotSpeed()
{
  if(abs(_linearX) > _maxLinearX)
  {
    if(_linearX < 0)
      _linearX = -_maxLinearX;
    else
      _linearX = _maxLinearX;
  }
  if(abs(_angularZ) > _maxAngularZ)
  {
    if(_angularZ < 0)
      _angularZ = -_maxAngularZ;
    else
      _angularZ = _maxAngularZ;
  }
  geometry_msgs::Twist cmdMsg;
  cmdMsg.linear.x = _linearX;
  cmdMsg.angular.z = _angularZ;
  _cmdVelPub.publish(cmdMsg);

}


//***********************get set*********************//

void QNode::getRobotSpeed(double* linearX, double* anglarZ)
{
  *linearX = _OdomLinearX;
  *anglarZ = _OdomAngularZ;
}

std::vector<double> QNode::getRobotPose()
{
  std::vector<double> tempPose(_robotPose);
  return tempPose;
}

std::vector<double> QNode::getMapOrigin()
{
  std::vector<double> tempOrigin(_mapOrigin);
  return tempOrigin;
}

bool QNode::getGlobalPlan(QList<std::pair<double, double> >& plan)
{
  if(_globalPlan.isEmpty()) return false;
  plan.clear();
  int size = _globalPlan.size();
  for(int idx=0; idx < size; idx++)
  {
    plan.append(_globalPlan[idx]);
//    qDebug() << plan[idx].first << plan[idx].second;
  }
  return true;
//  plan.operator =(_globalPlan);
}

///
/// \brief setOperationMode
/// \param mode
///
void QNode::setOperationMode(OprationMode mode)
{
  _oprationMode = mode;
//    qDebug() << _oprationMode;
  std_msgs::Bool msg;
  msg.data = _oprationMode;
  _oprationModePub.publish(msg);
}

QNode::OprationMode QNode::getOprationMode()
{

  OprationMode tempMode = _oprationMode;
//  qDebug() << tempMode;
  return tempMode;
}

bool QNode::setManualCmd(ManualCmd cmd)
{
  if(_oprationMode == Manual)
  {
    switch(cmd)
    {
      case Forward:
        _linearX+=0.1;
        break;
      case Backward:
        _linearX-=0.1;
        break;
      case LeftTurn:
        _angularZ+=0.1;
        break;
      case RightTurn:
        _angularZ-=0.1;
        break;
      case Stop:
        _linearX = 0.0;
        _angularZ = 0.0;
        break;

    }
    pubRobotSpeed();
    return true;
  }else
  {
    qDebug() << "not manual mode.";
    return false;
  }
}

int QNode::getBatt()
{
//  int tempBatt = _battPer;
  return _battPer;
}

}  // namespace rosgui


