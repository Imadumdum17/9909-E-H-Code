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
        //blue auton

        // chassis.moveToPoint(0, 24, 1000);/'[=i]
        // chassis.turnToHeading(180, 1000);
        // chassis.moveToPose(0, 24, 180, 1000);


        // - CURENT AUTO -
        intake.move(20); //prevents blocks from going too far up
        intake2.move(127);
        chassis.setPose(0,0,90);
        chassis.moveToPoint(14.133, 0.533, 5000, {.maxSpeed=80, .minSpeed = 32});
        chassis.moveToPoint(14.133, 0.533, 5000, {.maxSpeed=80, .minSpeed = 32});
        chassis.moveToPoint(25.666, 0.2, 5000, {.maxSpeed=80, .minSpeed = 32});
        chassis.moveToPoint(32.799, 0 ,5000,{.maxSpeed=80, .minSpeed = 40});
        chassis.moveToPoint(39.5, -2.5, 5000, {.maxSpeed=80, .minSpeed = 40});
        tounge.extend();
        pros::delay(1500);
        tounge.retract();
        chassis.moveToPoint(36.5, -8, 5000, {.forwards = false, .maxSpeed=80, .minSpeed = 40});
        chassis.moveToPose(51.5, 2.5, 45, 5000);
        pros::delay(1000);
        intake2.move(-127);      
        intake.move(-127);


        // chassis.moveToPose(51, 5, 45, 5000); //REMEMBER TO SET MINSPEED
        // pros::delay(2500);
        // intake2.move(-127);      
        // intake.move(-127);
        // pros::delay(4500);
        // intake2.move(127);
        // intake.move(127);
        // pros::delay(500);
        // intake2.move(-127);
        // intake.move(-127);

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