import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
import xacro 

def generate_launch_description():
    # Ścieżka do pakietu robot_model
    pkg_path = get_package_share_directory('robot_model')
    xacro_file = os.path.join(pkg_path, 'urdf', 'robot.urdf.xacro')
    
    # Przetwórz plik xacro na URDF
    robot_description_config = xacro.process_file(xacro_file)
    robot_desc = robot_description_config.toxml()
    
    # Parametry - nazwa parametru to 'robot_description' (standard ROS 2)
    params = {'robot_description': robot_desc, 'use_sim_time': False}
    
    # Node robot_state_publisher
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[params]
    )
    
    # Node joint_state_publisher_gui
    joint_state_publisher_gui_node = Node(
        package='joint_state_publisher_gui',
        executable='joint_state_publisher_gui',
        output='screen'
    )
    
    # RViz
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen'
    )
    
    return LaunchDescription([
        robot_state_publisher_node,
        joint_state_publisher_gui_node,
        rviz_node
    ])