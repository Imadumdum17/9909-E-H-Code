#include "lemlib/chassis/chassis.hpp"
#include "pros/adi.hpp"
#include "pros/imu.hpp"
#include "pros/misc.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
extern lemlib::Chassis chassis;
extern pros::Controller master;
extern pros::MotorGroup left_mg;
extern pros::MotorGroup right_mg;
extern pros::Imu imu;
extern pros::Motor intake;
extern pros::Motor intake2;
extern pros::adi::Pneumatics tounge;
extern pros::adi::Pneumatics scythe;
extern lemlib::Drivetrain drivetrain;
extern lemlib::OdomSensors sensors;
extern lemlib::ControllerSettings lateral_controller;
extern lemlib::ControllerSettings angular_controller;
extern int drive;
extern int auton;
extern int start;
extern void on_center_button();
extern void on_right_button();
extern void on_left_button();
extern bool pressed;
extern bool pressed2;
