For the Task 2 (ROS 2), you need to navigate to ~ros2_ws/src/fastbot_waypoints and then you can try the different tests for a 
passing or failing result.



---> First, for the success result

On the fastbot_waypoints/test/test_waypoints.cpp file you need to be sure that the goal position is reachable for the robot, for example:
        expected_x = 0.30
        expected_y = 0.0

This is located on the lines 99 and 115 of the code. This goal position was defined because it is reachable within the map.
So now, you can try the simulation with this test. You need to open 3 terminals.

On the terminal #1, you need to start the simulation, but first of all, you need to compile:

cd ros2_ws
colcon build

and then, you can start it:

source ~/ros2_ws/install/setup.bash
ros2 launch fastbot_gazebo one_fastbot_room.launch.py

Then, when the simulation is running, open a second terminal. You need to navigate to the ~ros2_ws/ directory and then type:

source install/setup.bash
ros2 run fastbot_waypoints fastbot_action_server

The last command will start the Action server. Now, open a third terminal, navigate to the ~ros2_ws/ directory and then type:

source install/setup.bash
colcon test --packages-select fastbot_waypoints --event-handler=console_direct+
echo "Summary: $(colcon test-result --all | grep "test_waypoints.gtest.xml" | sed 's/.*: //')"

The second command will start the test and you will see the robot moving in the simulation window.
You also will get this successful result by typing the last command:

Summary: 2 tests, 0 errors, 0 failures, 0 skipped




 ---> For the failling result

On the fastbot_waypoints/test/test_waypoints.cpp file you need to modify the goal position to negative values, for example:
        expected_x = -0.30
        expected_y = 0.0

This is located on the lines 99 and 115 of the code. This goal position is negative, which makes the test fail.
So now, you can try the simulation with this test. You need to open 3 terminals.

Then, the process is the same:

Terminal #1:
cd ros2_ws
colcon build
source ~/ros2_ws/install/setup.bash
ros2 launch fastbot_gazebo one_fastbot_room.launch.py

Terminal #2:
cd ros2_ws
source install/setup.bash
ros2 run fastbot_waypoints fastbot_action_server

Terminal #3:
cd ros2_ws
source install/setup.bash
colcon test --packages-select fastbot_waypoints --event-handler=console_direct+
echo "Summary: $(colcon test-result --all | grep "test_waypoints.gtest.xml" | sed 's/.*: //')"

You will get the failling result:

Summary: 2 tests, 0 errors, 2 failures, 0 skipped


For both tests, do not type the command "colcon test-result --all" because it will show several test results and not only the expected within this task.