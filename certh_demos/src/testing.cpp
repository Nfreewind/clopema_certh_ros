#include <robot_helpers/Robot.h>
#include <robot_helpers/Utils.h>
#include <robot_helpers/Geometry.h>

using namespace std;

using namespace robot_helpers;
using namespace Eigen ;

int main(int argc, char **argv) {

    ros::init(argc, argv, "test");
    ros::NodeHandle nh;

    float tableHeight = 0.685 ; //0.695

    addBoxToCollisionModel(0, -1.1, tableHeight/2.0, 0.8, 0.8, tableHeight);

    geometry_msgs::Pose pose ;


    pose.position.x =  -0.2;
    pose.position.y =  -1.1 ;
    pose.position.z = 0.8;




    float a = 0.25 ;
    Eigen::Matrix3d orientation;

// front - face down
//    orientation << 0, 1, 0,
//                 sin(a), 0,  -cos(a),
//                -cos(a), 0, -sin(a) ;

//        // left face down
    orientation << sin(a), 0, -cos(a),
                    0, -1, 0,
                    -cos(a), 0, -sin(a) ;

    pose.orientation = rotationMatrix3ToQuaternion(orientation) ;

    moveArm(pose, "r2");


    pose.position.x =  0;
    pose.position.y =  -1.1 ;
    pose.position.z = 0.735;

    //Create plan
    clopema_arm_navigation::ClopemaMotionPlan mp;
    MoveRobot cmove;

    cmove.setServoMode(false);
    mp.request.motion_plan_req.group_name =  "r2_arm";
    mp.request.motion_plan_req.allowed_planning_time = ros::Duration(10);

    //Set start state
    getRobotState(mp.request.motion_plan_req.start_state);

    arm_navigation_msgs::SimplePoseConstraint desired_pose;


    desired_pose.header.frame_id = "base_link";
    desired_pose.header.stamp = ros::Time::now();
    desired_pose.link_name =  "r2_ee";
    desired_pose.pose = pose;
    desired_pose.absolute_position_tolerance.x = 0.0002;
    desired_pose.absolute_position_tolerance.y = 0.0002;
    desired_pose.absolute_position_tolerance.z = 0.0002;
    desired_pose.absolute_roll_tolerance = 0.1;
    desired_pose.absolute_pitch_tolerance = 0.1;
    desired_pose.absolute_yaw_tolerance = 0.1;

    poseToClopemaMotionPlan(mp, desired_pose);

    ROS_INFO("Planning");
    if (!plan(mp)){
        resetCollisionModel() ;
        return -1 ;
    }

    ROS_INFO("Executing");
    control_msgs::FollowJointTrajectoryGoal goal;
    goal.trajectory = mp.response.joint_trajectory;
    cmove.doGoal(goal);
    setServoPowerOff();
   // resetCollisionModel() ;
    return 0;





}

