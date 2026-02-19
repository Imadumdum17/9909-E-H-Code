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

// create the chassis as pointer for runtime replacement
lemlib::Chassis* chassis = nullptr;

// (re)create the chassis with current PID values
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

bool debugPage = true;
bool updateRequired = false;


void initialize() {
    // create chassis with current settings
    applyControllerSettings();
    chassis_mutex.take();
    if (chassis) chassis->calibrate();
    chassis_mutex.give();
	pros::lcd::initialize();

//create a task to not starve main thread of resources
    pros::Task screen_task([&]() {
        while (true) {
            // print robot location to the brain screen
            chassis_mutex.take();
            if (chassis) {
                auto p = chassis->getPose();
                pros::lcd::print(0, "X: %f", p.x); // x pos
                pros::lcd::print(1, "Y: %f", p.y); // y pos
                pros::lcd::print(2, "Theta: %f", p.theta); // heading
                if (debugPage) { // print PID values on two pages due to screen space constraints
                    pros::lcd::print(4, "lateral kP: %f", lateral_controller.kP); //lateral kP
                    pros::lcd::print(5, "lateral kI: %f", lateral_controller.kI); //lateral kI
                    pros::lcd::print(6, "lateral kD: %f", lateral_controller.kD); //lateral kD
                } else {
                    pros::lcd::print(4, "angular kP: %f", angular_controller.kP); //angualar kP
                    pros::lcd::print(5, "angular kI: %f", angular_controller.kI); //angualar kI
                    pros::lcd::print(6, "angular kD: %f", angular_controller.kD); //angualar kD
                }

                if (updateRequired) { //detect if PID values have been changed and are not yet applied
                    pros::lcd::set_text(8, "Update required");
                } else {
                    pros::lcd::clear_line(8);
                }
            }
            chassis_mutex.give();
            // pros::lcd::print(4, "Rotation Sensor: %i", horizontalencoder.get_position());
            // delay to save resources
            pros::delay(100);
        }
    });
}


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

        if (rightHeld and !xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            if (chassis) chassis->moveToPose(0, 24, 0, 5000);
        }
        if (rightHeld and !xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->moveToPose(0, 24, 0, 5000);
        }

        if (rightHeld and !xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->cancelAllMotions();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->moveToPose(0, 24, 0, 5000);
        }

        if (rightHeld and !xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->cancelAllMotions();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->moveToPose(0, 0, 0, 5000);
        }

        if (leftHeld and !xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            if (chassis) chassis->turnToHeading(90, 5000);
        }

        if (leftHeld and !xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->turnToHeading(90, 5000);
        }

        if (leftHeld and !xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->turnToHeading(90, 5000);
        }

        if (leftHeld and !xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
            chassis_mutex.take();
            if (chassis) chassis->setPose(0, 0, 0);
            chassis_mutex.give();
            // if (chassis) chassis->turnToHeading(90, 5000);
        }

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

        if (xHeld and !aHeld and !yHeld and !l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                angular_controller.kP += 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kP +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                angular_controller.kP -= 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kP -1");
            }
        }

        if (xHeld and aHeld and !yHeld and !l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                lateral_controller.kP += 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kP +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                lateral_controller.kP -= 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kP -1");
            }
        }

        
        if (xHeld and !aHeld and yHeld and !l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                angular_controller.kP += 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kP +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                angular_controller.kP -= 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kP -0.1");
            }
        }

        if (xHeld and aHeld and yHeld and !l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                lateral_controller.kP += 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kP +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                lateral_controller.kP -= 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kP -0.1");
            }
        }


        if (xHeld and !aHeld and !yHeld and !l2Held and r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                angular_controller.kD += 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kD +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                angular_controller.kD -= 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kD -1");
            }
        }

        if (xHeld and aHeld and !yHeld and !l2Held and r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                lateral_controller.kD += 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kD +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                lateral_controller.kD -= 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kD -1");
            }
        }

        
        if (xHeld and !aHeld and yHeld and !l2Held and r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                angular_controller.kD += 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kD +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                angular_controller.kD -= 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kD -0.1");
            }
        }

        if (xHeld and aHeld and yHeld and !l2Held and r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                lateral_controller.kD += 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kD +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                lateral_controller.kD -= 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kD -0.1");
            }
        }


        if (xHeld and !aHeld and !yHeld and l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                angular_controller.kI += 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kI +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                angular_controller.kI -= 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kI -1");
            }
        }

        if (xHeld and aHeld and !yHeld and l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                lateral_controller.kI += 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kI +1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                lateral_controller.kI -= 1.0f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kI -1");
            }
        }

        
        if (xHeld and !aHeld and yHeld and l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                angular_controller.kI += 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kI +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                angular_controller.kI -= 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "angular kI -0.1");
            }
        }

        if (xHeld and aHeld and yHeld and l2Held and !r2Held) {
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
                lateral_controller.kI += 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kI +0.1");
            }
            if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
                lateral_controller.kI -= 0.1f;
                updateRequired = true;
                pros::lcd::set_text(9, "lateral kI -0.1");
            }
        }
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

        if (xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
            applyControllerSettings();
            updateRequired = false;
        }

        
        if (xHeld and master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
            debugPage = !debugPage;
        }

        if (not master.get_digital(pros::E_CONTROLLER_DIGITAL_R2) and not master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) { intake2.brake(); }
        if (not master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) and not master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) { intake.brake(); }


        if (toggle == true) {
            intake.move(20);
        }
	
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