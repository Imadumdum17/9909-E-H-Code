#include "misc.h"
#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "pros/rtos.hpp"

//drivetrain is 14.5 x 18 for some dumbass reason

void motors() { //unused
    intake.move(20); //prevents blocks from going too far up
    intake2.move(127); //intake three
    pros::delay(1500);
    intake2.move(-127); //outtake three + preload to bottom goal
    pros::delay(1500);
}

void autonomous() {
    //pros::Task autointake(motors);
    if (auton == 0) {
        //red auton
        /*
        //i have to redo all of these measurements FUCKKKKKK
        chassis.setPose(-61.171, -17.226, 5000);
        intake.move(30); //prevents blocks from going too far up
        intake2.move(127); //intake three
        chassis.moveToPoint(32.266, -4, 5000); //go to three    
        chassis.moveToPoint(46.932, 2.4, 5000); //go to bottom goal
        intake2.move(-127); //outtake three + preload to bottom goal
        pros::delay(1000);
        chassis.moveToPoint(34.932, 13.866, 5000); //leave bottom goal
        intake2.move(127); //intake three
        chassis.moveToPoint(36.266, 32.799, 5000); //go to left three
        chassis.moveToPoint(48.265, 32.799, 5000); //go to midgoal
        intake.move(127); //score three midgoal
        pros::delay(1000);
        intake.move(30); //block blocker
        chassis.moveToPoint(51.465, 41.599, 5000); //leave midgoal
        chassis.moveToPoint(23.466, 54.932, 5000); //continue towards left loader
        tounge.extend(); //extend tounge (no way)
        chassis.moveToPoint(-55.838, 46.505, 5000); //slurp three from loader
        pros::delay(1000); //adjust depending on slurp time
        chassis.moveToPoint(-33.172, 47.305, 5000, {false}); //go to longgoal
        intake.move(-127); //score three longgoal
        */

    }
    else if (auton == 1) {
    chassis.setPose(0, 0, 0);
    //chassis.moveToPoint(0, -16, 5000, {.forwards = true});
    // chassis.moveToPoint(0, 38.05, 5000, {.forwards = false, .maxSpeed = 89});
    // chassis.moveToPoint(13.13, 50.108, 5000, {.forwards = false});
    intake.move(127);
    intake2.move(127);
    chassis.moveToPose(18, 32, 270, 2000);
    tounge.extend();
    chassis.moveToPose(-6, 32, 270, 5000, {.minSpeed = 50});
    scythe.extend();
    pros::delay(1000);
    chassis.moveToPose(0, 32, 270, 5000, {.forwards = false, .minSpeed = 50});
    pros::delay(500);
    chassis.moveToPose(-6, 32, 270, 5000, {.minSpeed = 50});
    pros::delay(1000);
    chassis.moveToPose(0, 32, 270, 5000, {.forwards = false, .minSpeed = 50});
    pros::delay(500);
    chassis.moveToPose(-6, 32, 270, 5000, {.minSpeed = 50});
    pros::delay(5000);
    chassis.moveToPose(-14, -12, 0, 5000, {.forwards = false, .minSpeed = 80});
    chassis.moveToPose(-14, -21, 0, 5000, {.forwards = false, .minSpeed = 80});
    tounge.retract();
    // chassis.moveToPose(24, 32.5, 270, 5000);
    //chassis.moveToPose(-8.378, 68, 270, 5000,{.forwards = true, .minSpeed = 67});
    // pros::delay(1000);
    // // dt.move(127);
    // pros::delay(1500);
    // intake.brake();
    // pros::delay(1000);
    // // dt.move(-127);
    // pros::delay(100);
    // // dt.move(127);
    // pros::delay(1500);
    // dt.brake();
    // chassis.moveToPose(-28 ,60, 270, 5000, {.forwards = false, .maxSpeed = 67, .minSpeed = 67});
    // pros::delay(500);
    // intake.move(67);
    // pros::delay(1500);
    // chassis.moveToPose(-20 ,20, 360, 5000, {.forwards = true, .maxSpeed = 67, .minSpeed = 67});

    //1.1s front>back

    // chassis.moveToPose(-24.378, 67.5, 270, 5000,{.forwards = true, .minSpeed = 67});
    // chassis.moveToPose(-16.378, 67.5, 270, 5000,{.minSpeed = 67});
        //blue auton
        //pros::delay(1500);
        //chassis.turnToHeading(90, 5000, {lemlib::AngularDirection::CW_CLOCKWISE});
        //pros::delay(1500);
        //chassis.moveToPoint(24, -24, 5000, {true, 67});

        //chassis.moveToPoint(0, -67, 5000, {true});  // - SKILLS -
        //chassis.moveToPose(-8, 31, 45, 5000, {false});
   //     chassis.moveToPoint(0, 22, 5000, {false});
    //    intake.move(-127); //score midgoal
        // a/b = 15.5
    }
}