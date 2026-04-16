FROM osrf/ros:humble-desktop

ENV DEBIAN_FRONTEND=noninteractive

# =========================
# Dependencias base + herramientas ROS2
# =========================
RUN apt-get update && apt-get install -y \
    python3-colcon-common-extensions \
    python3-rosdep \
    python3-vcstool \
    build-essential \
    git \
    x11-apps \
    gazebo \
    ros-humble-gazebo-ros-pkgs \
    ros-humble-gazebo-ros \
    ros-humble-gazebo-plugins \
    ros-humble-xacro \
    ros-humble-joint-state-publisher \
    ros-humble-joint-state-publisher-gui \
    ros-humble-robot-state-publisher \
    && rm -rf /var/lib/apt/lists/*

# =========================
# Inicializar rosdep
# =========================
RUN rosdep init || true
RUN rosdep update

# =========================
# Crear workspace ROS2
# =========================
RUN mkdir -p /root/ros2_ws/src
WORKDIR /root/ros2_ws

# =========================
# Copiar paquetes
# (asegúrate de tener docker_src/)
# =========================
COPY ./docker_src /root/ros2_ws/src

# =========================
# Instalar dependencias (más robusto)
# =========================
RUN /bin/bash -c "source /opt/ros/humble/setup.bash && \
    apt-get update && \
    rosdep install --from-paths src --ignore-src -r -y"

# =========================
# Compilar workspace
# =========================
RUN /bin/bash -c "source /opt/ros/humble/setup.bash && \
    colcon build --symlink-install"

# =========================
# Configurar entorno automático
# =========================
RUN echo 'source /opt/ros/humble/setup.bash' >> ~/.bashrc && \
    echo 'source /root/ros2_ws/install/setup.bash' >> ~/.bashrc

# =========================
# Variables para CI (Gazebo)
# =========================
ENV QT_X11_NO_MITSHM=1
ENV DISPLAY=:99

WORKDIR /root/ros2_ws

CMD ["/bin/bash"]
