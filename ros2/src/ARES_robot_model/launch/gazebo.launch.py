import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, Command
from launch_ros.actions import Node 


def generate_launch_description():
    # Define the robot's name and package name
    robot_name = "ARES_robot_model"
    package_name = "ARES_robot_model"

    # Set paths
    pkg_path = get_package_share_directory(package_name)
    ros_gz_sim_pkg = get_package_share_directory('ros_gz_sim')

    # Path to Xacro file
    robot_model_path = os.path.join(pkg_path, 'urdf', 'robot.urdf.xacro')

    # Path to Bridge config
    gz_bridge_params_path = os.path.join(pkg_path, 'config', 'gz_bridge.yaml')

    # --- Launch Arguments ---
    world_arg = DeclareLaunchArgument(
        'world',
        default_value='empty.sdf',
        description='Specify the world file for Gazebo (e.g., empty.sdf)'
    )

    x_arg = DeclareLaunchArgument('x', default_value='0.0', description='Initial X position')
    y_arg = DeclareLaunchArgument('y', default_value='0.0', description='Initial Y position')
    z_arg = DeclareLaunchArgument('z', default_value='0.5', description='Initial Z position')

    # Retrieve launch configurations
    world_file = LaunchConfiguration('world')

    # --- Fix 1: Use Command Substitution for Xacro ---
    # To jest bezpieczniejsza metoda niż xacro.process_file() wewnątrz launch file
    robot_description = Command(['xacro ', robot_model_path])

    # --- Fix 2: Correct Launch File Name for Gazebo ---
    # W ROS 2 Jazzy plik nazywa się gz_sim.launch.py, a nie gazebo.launch.py
    gazebo_pkg_launch = PythonLaunchDescriptionSource(
        os.path.join(ros_gz_sim_pkg, 'launch', 'gz_sim.launch.py')
    )

    # Include the Gazebo launch description
    gazebo_launch = IncludeLaunchDescription(
        gazebo_pkg_launch,
        launch_arguments={
            'gz_args': ['-r -v 4 ', world_file],
            'on_exit_shutdown': 'true'
        }.items()
    )

    # Spawn robota w Gazebo
    spawn_model_gazebo_node = Node(
        package='ros_gz_sim',
        executable='create',
        arguments=[
            '-name', robot_name,
            '-string', robot_description,
            '-x', '0.0',
            '-y', '0.0',
            '-z', '0.2',
            '-allow_renaming', 'false'
        ],
        output='screen'
    )

    # Robot State Publisher
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[
            {'robot_description': robot_description, 'use_sim_time': True}
        ],
        output='screen'
    )

    # ROS-Gazebo Bridge
    gz_bridge_node = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=[
            '--ros-args', '-p',
            f'config_file:={gz_bridge_params_path}'
        ],
        output='screen'
    )

    rviz_file = os.path.join(
        get_package_share_directory(package_name),
        'rviz',
        'ARES_model.rviz'
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_file]
    )

    teleop_node = Node(
        package='teleop_twist_keyboard',
        executable='teleop_twist_keyboard',
        name='teleop',
        prefix='gnome-terminal --', 
        output='screen'
    )

    return LaunchDescription([
        world_arg,
        x_arg,
        y_arg,
        z_arg,
        gazebo_launch,
        spawn_model_gazebo_node,
        robot_state_publisher_node,
        gz_bridge_node,
        rviz_node,
        teleop_node,
    ])