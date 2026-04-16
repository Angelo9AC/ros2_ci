### Github repository

https://github.com/Angelo9AC/ros2_ci

### For Docker

You need to install docker in the rosject with the following commands:

sudo apt-get update
sudo apt-get install -y docker.io docker-compose
sudo service docker start

Then, you can use docker without sudo with:

sudo usermod -aG docker $USER
newgrp docker

Then, the following command is important to access to the simulation environment within the docker container:

sudo apt install x11-xserver-utils

Then, go to the cd ros2_ws/src/ros2_ci directory and type:

docker build -t test_ros .

docker run -it \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  test_ros

Within the container, you need to type:

source install/setup.bash
ros2 launch fastbot_gazebo one_fastbot_room.launch.py

If at first it doesn't open the simulation, then try it again.

### For Jenkins

In a second terminal, you need to type the following commands:

cd ~/webpage_ws
bash start_jenkins.sh

Then, you need to go to the jenkins__pid__url.txt file and copy the link that is inside it and paste it in your browser. You will be able to enter to the jenkins webpage and access with the following user and password:

In my case is:

username: admin
password: #Tech2020

Then you will be able to see the control panel, access to the ros1_ci_build work and start the building process.