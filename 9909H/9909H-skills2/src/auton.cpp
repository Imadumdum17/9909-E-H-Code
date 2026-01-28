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
       // chassis.setPose(0, 0, 180);
        //chassis.moveToPoint(20.37, 64.705, 5000, {.forwards = false});
        //scythe.extend();
        //chassis.moveToPoint(0.599, 23.366, 5000, {.forwards = false});

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
    //45 Auton Skills
//    scythe.extend();
//    chassis.setPose(0, 0, 0);
//     chassis.moveToPoint(1.039, -42.658, 5000, {.forwards = false});
//     chassis.moveToPoint(31.101, -48.503, 5000);
//     chassis.moveToPoint(0.204, -66.039, 5000);
//     chassis.moveToPoint(30.544, -76.059, 5000);
//     chassis.moveToPoint(107.368, -72.998, 5000);
//     chassis.moveToPoint(124.904, -66.874, 5000);
//     chassis.moveToPoint(123.225, 10.953, 5000);
//     chassis.moveToPoint(125.173, 29.046, 5000);
//     chassis.moveToPoint(109.864, 43.52, 5000);
//     chassis.moveToPoint(33.959, 42.587, 5000);
//     chassis.moveToPoint(22.463, 32.665, 5000);
//     chassis.moveToPoint(-0.918, 27.376, 5000);
//     chassis.moveToPoint(9.381, 12.345, 5000);
//     chassis.moveToPoint(3.619, 4.454, 5000);
//     chassis.moveToPoint(-0.557, -15.866, 5000);




//Auton skills 20 point
left_mg.move(-127) && right_mg.move(-127);
pros::delay(1000);
left_mg.brake() && right_mg.brake();
left_mg.move(127) && right_mg.move(127);
pros::delay(1000);
left_mg.brake() && right_mg.brake();


//Auton Skills PATH CODE 20 Point

chassis.setPose(0, 0, 5000);
chassis.moveToPoint(0.536, -36.71, 5000, {.forwards = false});
chassis.moveToPoint(26.528, -46.625, 5000, {.forwards = false});
chassis.moveToPoint(32.155, -31.887, 5000, {.forwards = false});
chassis.moveToPoint(25.992, -16.613, 5000, {.forwards = false});
chassis.moveToPoint(0.804, -17.685, 5000, {.forwards = false});
}
