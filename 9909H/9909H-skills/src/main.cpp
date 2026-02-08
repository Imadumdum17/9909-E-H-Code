#include "main.h"
#include "liblvgl/llemu.hpp"
#include "pros/imu.hpp"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "pros/misc.hpp"
#include "pros/motors.hpp"

//negative number means reversed motor
pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::MotorGroup left_mg({-1, -2, -3}, pros::MotorGearset::blue); // left motors on ports 1, 2, 3
pros::MotorGroup right_mg({4, 5, 6}, pros::MotorGearset::blue); // right motors on ports 4, 5, 6
pros::Imu imu(20);
pros::Motor intake(-7);
pros::Motor intake2(8);
pros::Rotation horizontalencoder(19);

pros::adi::Pneumatics tounge('A',false);
pros::adi::Pneumatics scythe('B',false);

lemlib::TrackingWheel horizontal_tracking_wheel(&horizontalencoder, lemlib::Omniwheel::NEW_2, 0.25);

// drivetrain settings
lemlib::Drivetrain drivetrain(&left_mg, // left motor group
                              &right_mg, // right motor group
                              10, // 10 inch track width
                              3.25, // using new 4" omnis
                              450, // drivetrain rpm is 360
                              8
);

//odom sensors
lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel 1
                            nullptr, // vertical tracking wheel 2
                            &horizontal_tracking_wheel, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2
                            &imu // inertial sensor
);


// lateral PID controller
lemlib::ControllerSettings lateral_controller(10, // proportional gain (kP) [2]
                                              0, // integral gain (kI) [0]
                                              4, // derivative gain (kD) [10]
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              20 //2 maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angular_controller(3.5, // proportional gain (kP) [10] 5
                                              0, // integral gain (kI) 0
                                              13, // derivative gain (kD) [14]
                                              3, // anti windup 3
                                              1, // small error range, in degrees 1
                                              100, // small error range timeout, in milliseconds 100
                                              3, // large error range, in degrees 3
                                              500, // large error range timeout, in milliseconds 500
                                              0 // maximum acceleration (slew) 0
);

// create the chassis as a pointer so it can be replaced at runtime
lemlib::Chassis* chassis = nullptr;

// (re)create the chassis with current controller settings
static pros::Mutex chassis_mutex;
void applyControllerSettings() {
    chassis_mutex.take();
    // delete old chassis if it exists
    if (chassis != nullptr) {
        delete chassis;
        chassis = nullptr;
    }
    // define new chassis with current settings
    chassis = new lemlib::Chassis(drivetrain, lateral_controller, angular_controller, sensors);
    chassis_mutex.give();
}

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
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    // create chassis with current settings
    applyControllerSettings();
    chassis_mutex.take();
    if (chassis) chassis->calibrate();
    chassis_mutex.give();
	pros::lcd::initialize();
	pros::lcd::set_text(0, "curvature");
    pros::lcd::set_text(1, "red auton");

    pros::lcd::register_btn0_cb(on_left_button);
	// pros::lcd::register_btn1_cb(on_center_button);
    pros::lcd::register_btn2_cb(on_right_button);
    pros::Task screen_task([&]() {
        while (true) {
            // print robot location to the brain screen
            chassis_mutex.take();
            if (chassis) {
                auto p = chassis->getPose();
                pros::lcd::print(0, "X: %f", p.x); // x
                pros::lcd::print(1, "Y: %f", p.y); // y
                pros::lcd::print(2, "Theta: %f", p.theta); // heading
            }
            chassis_mutex.give();
            // pros::lcd::print(4, "Rotation Sensor: %i", horizontalencoder.get_position());
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
        bool xHeld = master.get_digital(pros::E_CONTROLLER_DIGITAL_X);
        bool yHeld = master.get_digital(pros::E_CONTROLLER_DIGITAL_Y);
        bool aHeld = master.get_digital(pros::E_CONTROLLER_DIGITAL_A);
        bool l2Held = master.get_digital(pros::E_CONTROLLER_DIGITAL_L2);
        bool r2Held = master.get_digital(pros::E_CONTROLLER_DIGITAL_R2);
        bool leftHeld = master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT);
        bool rightHeld = master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT);


        if (!xHeld and !leftHeld and !rightHeld) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) { intake2.move(127);}
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) { intake2.move(-127);}

            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) { intake.move(127); toggle = false;}
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) { intake.move(-127); toggle = false;}
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) { tounge.toggle(); }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) { scythe.toggle(); }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)) { toggle = true; }
            
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) { curve = !curve; }
        }

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

        if (rightHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            if (chassis) chassis->moveToPose(0, 24, 0, 5000);
        }

        if (rightHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->moveToPose(0, 24, 0, 5000);
        }

        if (rightHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->moveToPose(0, 24, 0, 5000);
        }

        if (rightHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->moveToPose(0, 0, 0, 5000);
        }

        if (leftHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            if (chassis) chassis->turnToHeading(90, 5000);
        }

        if (leftHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->turnToHeading(90, 5000);
        }

        if (leftHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->turnToHeading(90, 5000);
        }

        if (leftHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->turnToHeading(90, 5000);
        }

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

        if (xHeld and !aHeld and !yHeld and !l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                angular_controller.kP += 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kP +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                angular_controller.kP -= 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kP -1");
            }
        }

        if (xHeld and aHeld and !yHeld and !l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                lateral_controller.kP += 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kP +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                lateral_controller.kP -= 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kP -1");
            }
        }

        
        if (xHeld and !aHeld and yHeld and !l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                angular_controller.kP += 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kP +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                angular_controller.kP -= 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kP -0.1");
            }
        }

        if (xHeld and aHeld and yHeld and !l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                lateral_controller.kP += 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kP +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                lateral_controller.kP -= 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kP -0.1");
            }
        }


        if (xHeld and !aHeld and !yHeld and !l2Held and r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                angular_controller.kD += 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kD +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                angular_controller.kD -= 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kD -1");
            }
        }

        if (xHeld and aHeld and !yHeld and !l2Held and r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                lateral_controller.kD += 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kD +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                lateral_controller.kD -= 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kD -1");
            }
        }

        
        if (xHeld and !aHeld and yHeld and !l2Held and r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                angular_controller.kD += 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kD +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                angular_controller.kD -= 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kD -0.1");
            }
        }

        if (xHeld and aHeld and yHeld and !l2Held and r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                lateral_controller.kD += 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kD +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                lateral_controller.kD -= 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kD -0.1");
            }
        }


        if (xHeld and !aHeld and !yHeld and l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                angular_controller.kI += 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kI +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                angular_controller.kI -= 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kI -1");
            }
        }

        if (xHeld and aHeld and !yHeld and l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                lateral_controller.kI += 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kI +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                lateral_controller.kI -= 1.0f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kI -1");
            }
        }

        
        if (xHeld and !aHeld and yHeld and l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                angular_controller.kI += 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kI +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                angular_controller.kI -= 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "angular kI -0.1");
            }
        }

        if (xHeld and aHeld and yHeld and l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
                lateral_controller.kI += 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kI +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
                lateral_controller.kI -= 0.1f;
                applyControllerSettings();
                pros::lcd::set_text(4, "lateral kI -0.1");
            }
        }
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

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

        //why does this crash the brain what
	
        int leftY = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
		int rightX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        int rightY = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);

        chassis_mutex.take();
        if (chassis) chassis->curvature(leftY, rightX);
        chassis_mutex.give();


		// Arcade control scheme
		pros::delay(20); // run for 20 ms then update. saves resources by not running a ludicrious amount of times per second.
	}
}