include_directories(${PROJECT_TOP_SOURCE_DIR} ${Boost_INCLUDE_DIRS} ${MAVLINK_INCLUDE_DIRS})

add_executable(mavlinkHilState mavlinkHilState.cpp)
add_dependencies(mavlinkHilState  mavlink ${PROJECT_NAME}Libs)
target_link_libraries(mavlinkHilState ${${PROJECT_NAME}Libs} ${Boost_LIBRARIES})
