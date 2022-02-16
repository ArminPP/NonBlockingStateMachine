

// non blocking delayed state machine with constant loop time
// ##########################################################
//
// based on the code from 'GoForSmoke' in the arduino forum
// https://forum.arduino.cc/t/fading-led-up-and-down-with-predetermined-intervals-non-blocking/559601/4

/*

                                          STATE MACHINE DIAGRAM

                 Task 1           Task 2             Task 3           Task n
                +-----+       +-----------+        +--------+       +-------+
                |     |       |           |        |        |       |       |
                |     |       |           |        |        |       |       |
                |     |       |           |        |        |       |       |
                |     |       |           |        |        |       |       |
                +     +-------+           +--------+        +-------+       +-----/ /------+

                |             |                    |                |                      |
                |             |                    |                |                      |
                |             |                    |                |                      |
                |<----------->|<------------------>|<-------------->|<-------------------->|
                |                                                                          |
                |   State 1          State 2             State 3       State n + last Task |
                |                                                                          |
                |                                                                          |
                |                                                                          |
                |<------------------------------------------------------------------------>|
                                       state machine interval time




             every state includes some blocking code (Task) and a nonblocking delay
             the last task fills the gap between runtime and interval time if necessary

*/

#include <Arduino.h>
// #include <M5Stack.h>

const uint32_t STATE_MACHINE_INTERVAL = 5000; // must be equal or larger than the sum of all delays!

const uint32_t DELAY_STATE_1 = 200;
const uint32_t DELAY_STATE_2 = 1000;
const uint32_t DELAY_STATE_3 = 1000;
const uint32_t DELAY_STATE_4 = 200;
const uint32_t DELAY_STATE_5 = 200;

enum STATES // different tasks in the state machine
{
  READ_SENSOR_1,
  READ_SENSOR_2,
  READ_SENSOR_3,
  READ_SENSOR_4,
  READ_SENSOR_5,

  LAST_TASK
};

byte STATE_NR; // state tracking
unsigned long STATE_START_DELAY = 0;
unsigned long STATE_WAIT_DELAY = 0;

unsigned long STATE_MACHINE_START = 0;
unsigned long STATE_MACHINE_END = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("\n\n\n  non blocking delayed state machine with constant loop time\n"));
  Serial.println(F("based on the code from 'GoForSmoke' in the arduino forum"));
  Serial.println(F("with some changes made by Armin Pressler (2022)\n"));
};

String runtime() // test
{
  String Time = "";
  String ms = "";
  unsigned long ss, MS;
  byte mm, hh;

  MS = millis();
  ss = millis() / 1000;
  hh = ss / 3600;
  mm = (ss - hh * 3600) / 60;
  ss = (ss - hh * 3600) - mm * 60;
  if (hh < 10)
    Time += "0";
  Time += (String)hh + ":";
  if (mm < 10)
    Time += "0";
  Time += (String)mm + ":";
  if (ss < 10)
    Time += "0";
  Time += (String)ss + ":";

  ms = (String)MS;
  if (MS >= 1000)
  {
    ms = ms.substring(ms.length() - 3);
  };

  Time += ms;
  return Time;
}

void NonBlockingDummyDelay() // a 2nd nonblocking function, only for test purposes
{
  static uint32_t next_request = millis();
  if (millis() - next_request >= 250)
  {
    delay(210);
    Serial.printf("dummy delay: %lu\n", millis() - next_request - 250);
    next_request = millis();
  }
}

void NonBlockingStateMachine()
{
  // start of one-shot timer
  if (STATE_WAIT_DELAY > 0) // one-shot timer only runs when set
  {
    if (millis() - STATE_START_DELAY < STATE_WAIT_DELAY)
    {
      return; // instead of blocking, the undelayed function returns
    }
    else
    {
      STATE_WAIT_DELAY = 0; // time's up! turn off the timer and run the STATE_NR case
    }
  }

  static unsigned long timer = millis();
  if (STATE_NR == 0)
  {
    Serial.printf("----- state machine interval: %lu\n", millis() - timer);
    timer = millis();
  }
  // end of one-shot timer

  switch (STATE_NR) // runs the case numbered in STATE_NR
  {
  case READ_SENSOR_1:
    STATE_MACHINE_START = millis();

    Serial.printf("StateNo:%2i delay: %lu\n", READ_SENSOR_1, millis() - STATE_START_DELAY);
    STATE_START_DELAY = millis(); // put some blocking code always AFTER this line

    delay(333); // simulates some blocking code

    STATE_WAIT_DELAY = DELAY_STATE_1; // for the next DELAY_STATE_1, this function will return on entry.
    STATE_NR++;                             // when the switch-case runs again it will be the next case that runs
    break;                                  // exit switch-case

  case READ_SENSOR_2:
    Serial.printf("StateNo:%2i delay: %lu\n", READ_SENSOR_2, millis() - STATE_START_DELAY);
    STATE_START_DELAY = millis();

    delay(1001); // If blocking code tooks longer than DELAY_STATE_2,

    STATE_WAIT_DELAY = DELAY_STATE_2;
    STATE_NR++;
    break;

  case READ_SENSOR_3:
    Serial.printf("StateNo:%2i delay: %lu\n", READ_SENSOR_3, millis() - STATE_START_DELAY);
    STATE_START_DELAY = millis();

   delay(1002); // simulates some blocking code

    STATE_WAIT_DELAY = DELAY_STATE_3;
    STATE_NR++;
    break;

  case READ_SENSOR_4:
    Serial.printf("StateNo:%2i delay: %lu\n", READ_SENSOR_4, millis() - STATE_START_DELAY);
    STATE_START_DELAY = millis();

    delay(199); // more blocking code

    STATE_WAIT_DELAY = DELAY_STATE_4;
    STATE_NR++;
    break;

  case READ_SENSOR_5:
    Serial.printf("StateNo:%2i delay: %lu\n", READ_SENSOR_5, millis() - STATE_START_DELAY);
    STATE_START_DELAY = millis();

   delay(33); // simulates some blocking code

    STATE_WAIT_DELAY = DELAY_STATE_5;
    STATE_NR++;
    break;

  // case READ_SENSOR_6:
  //   Serial.printf("StateNo:%2i delay: %lu\n", READ_SENSOR_6, millis() - STATE_START_DELAY);
  //   STATE_START_DELAY = millis();

  //   STATE_WAIT_DELAY = DELAY_STATE_6;
  //   STATE_NR++;
  //   break;

  case LAST_TASK:                                       // fill up the gap between runntime and looptime if necessary
    STATE_MACHINE_END = millis() - STATE_MACHINE_START; // calculate running time of state machine
    // be careful: Serial.println() takes about 3ms (at least with wokwi.com online sim) - so timing is not very accurate
    //      Serial.printf("[%s] Last task %2i delay: %i  time left:%i\n",
    //                     runtime(), LAST_TASK, millis() - STATE_START_DELAY, STATE_MACHINE_INTERVAL - STATE_MACHINE_END);
    STATE_START_DELAY = millis();

    Serial.printf("Looptime state machine: %lu ms\n", STATE_MACHINE_END);
    if (STATE_MACHINE_END <= STATE_MACHINE_INTERVAL) // if runtime smaller than looptime of state machine
    {
      STATE_WAIT_DELAY = STATE_MACHINE_INTERVAL - STATE_MACHINE_END;
      // be careful: Serial.println() takes about 3ms (at least with wokwi.com online sim) - so timing is not very accurate
      //        Serial.printf("compensate time: %i ms\n", STATE_WAIT_DELAY);
    }
    else
    {
      STATE_WAIT_DELAY = 0;
      Serial.printf("WARNING: blocking code took longer (%lu ms) than overall interval (%i ms) !\n",
                    STATE_MACHINE_END - STATE_MACHINE_INTERVAL, STATE_MACHINE_INTERVAL);
    }
    STATE_NR = 0; // finish, start again
    break;
  }
}

void loop()
{
  NonBlockingStateMachine();
  // NonBlockingDummyDelay();
  // delay(100); // INFO no delay() is allowed here!
}
