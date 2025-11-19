#include "main.h"
#include "liblvgl/llemu.hpp"
#include "pros/imu.hpp"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "pros/misc.hpp"

//negative number means reversed motor
pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::MotorGroup left_mg({-1, -2, -3}, pros::MotorGearset::blue); // left motors on ports 1, 2, 3
pros::MotorGroup right_mg({4, 5, 6}, pros::MotorGearset::blue); // right motors on ports 4, 5, 6
pros::Imu imu(20);
pros::Motor intake(-7);
pros::Motor intake2(8);

pros::adi::Pneumatics tounge('A',false);
pros::adi::Pneumatics scythe('B',false);


// drivetrain settings
lemlib::Drivetrain drivetrain(&left_mg, // left motor group
                              &right_mg, // right motor group
                              10, // 10 inch track width
                              3.25, // using new 4" omnis
                              450, // drivetrain rpm is 360
                              2 // horizontal drift is 2 (for now)
);

//use nullptr if you dont have tracking wheels
lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel 1
                            nullptr, // vertical tracking wheel 2
                            nullptr, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2
                            &imu // inertial sensor
);


// i dont want ot tune thisss fuckkkk
// lateral PID controller
lemlib::ControllerSettings lateral_controller(10, // proportional gain (kP)
                                              1, // integral gain (kI)
                                              3, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              20 // maximum acceleration (slew)
);

//i am going to fucking kill myself
// angular PID controller
lemlib::ControllerSettings angular_controller(2, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              10, // derivative gain (kD)
                                              3, // anti windup 3
                                              1, // small error range, in degrees 1
                                              100, // small error range timeout, in milliseconds 100
                                              3, // large error range, in degrees 3
                                              500, // large error range timeout, in milliseconds 500
                                              0 // maximum acceleration (slew) 0
);

// create the chassis
lemlib::Chassis chassis(drivetrain, // drivetrain settings
                        lateral_controller, // lateral PID settings
                        angular_controller, // angular PID settings
                        sensors // odometry sensors
);

bool curve = true;
int auton = 1;
int start = 0;
int uptime = 1;
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	/*static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(0, "curvature");
        drive = 1;
	} else {
		pros::lcd::set_text(0, "tank");
        drive = 0;
	} */
}

void on_right_button() {
    static bool pressed2 = false;
    pressed2 = !pressed2;
    if (pressed2) {
        pros::lcd::set_text(1, "blue auton");
        auton = 1;
    } else {
        pros::lcd::set_text(1, "red auton");
        auton = 0;
    }    
}

void on_left_button() {
    start = 1;
}
/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	chassis.calibrate();
	pros::lcd::initialize();
	pros::lcd::set_text(0, "curvature");
    pros::lcd::set_text(1, "red auton");

    //kill all pros devs KILL ALL PROS DEVS I HATE THEM
    pros::lcd::register_btn0_cb(on_left_button);
	// pros::lcd::register_btn1_cb(on_center_button);
    pros::lcd::register_btn2_cb(on_right_button);
    pros::Task screen_task([&]() {
        while (true) {
            // print robot location to the brain screen
            pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
            pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
            pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
            // delay to save resources
            pros::delay(20);
        }
    });
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}


void opcontrol() {
    bool toggle = false;
    bool toggle2 = false;

	while (true) {
		if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) { intake2.move(127);}
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) { intake2.move(-127);}

        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) { intake.move(127); toggle = false;}
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) { intake.move(-127); toggle = false;}
		if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) { tounge.toggle(); }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) { scythe.toggle(); }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)) { toggle = true; }
        
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) { curve = !curve; }
        //kill all ruiguan students i fucking hate them all

        if (not master.get_digital(pros::E_CONTROLLER_DIGITAL_R2) and not master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) { intake2.brake(); }
        if (not master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) and not master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) { intake.brake(); }


        if (toggle == true) {
            intake.move(20);
        }
        
		//pros::lcd::print(4, "X: %f", chassis.getPose().x); // x
        //pros::lcd::print(5, "Y: %f", chassis.getPose().y); // y
        //pros::lcd::print(6, "Theta: %f", chassis.getPose().theta); // heading
        //pros::lcd::print(3, "uptime: %f", uptime); // uptime
        //uptime = uptime + 1;

        //why the fuck does this crash the brain
	
        int leftY = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
		int rightX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        int rightY = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

		
		if (curve == true) {
			chassis.curvature(leftY, rightX);
		}
		else {
			//chassis.tank(leftY, rightY);
		}
        //i fucking hate every tank drive user fml


		// Arcade control scheme
		pros::delay(20); // Run for 20 ms then update. Savse resources by not running a ludicrious amount of times per second.
	}
}