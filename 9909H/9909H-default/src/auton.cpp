#include "misc.h"
#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep

void autonomous() {
    if (auton == 0) {
        //red auton
        chassis.moveToPoint(40.5, 0, 5000);
        //intake
        chassis.moveToPoint(-12, 0, 5000);
        chassis.moveToPose(0, 24, 90, 5000);
        //outtake

    }
    else if (auton == 1) {
        //blue auton
    }
}
