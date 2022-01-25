### NonBlockingStateMachine
non blocking delayed state machine with constant loop time


### Overview
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



- based on the code from 'GoForSmoke' in the arduino forum
- https://forum.arduino.cc/t/fading-led-up-and-down-with-predetermined-intervals-non-blocking/559601/4
