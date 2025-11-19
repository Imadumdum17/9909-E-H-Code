#include "misc.h"
#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "pros/rtos.hpp"


void motors() {
    intake.move(127);
    pros::delay(1000);
    intake2.move(127);
}

void autonomous() {
    pros::Task autointake(motors);
    if (auton == 0) {
        //red auton
    chassis.moveToPoint(-61.705, -17.226, 5000);
    chassis.moveToPoint(-27.039, -20.693, 5000);
    chassis.moveToPoint(-14.773, -12.96, 5000);

    
    


    }
    else if (auton == 1) {
        //blue auton
    }
}
