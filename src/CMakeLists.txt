include_directories(${Boost_INCLUDE_DIRS} ${MAVLINK_INCLUDE_DIRS})

set(${PROJECT_NAME}Libs ${Boost_LIBRARIES})

set(${PROJECT_NAME}Sources
	AsyncSerial.cpp
	asio_mavlink_bridge.cpp
	)

set(${PROJECT_NAME}Headers
	AsyncSerial.hpp 
	asio_mavlink_bridge.h
	)

if (SIMGEAR_FOUND AND PLIB_FOUND)
	list(APPEND ${PROJECT_NAME}Sources
		FlightGearComm.cpp
		flightGearUtils.cpp
		)
	list(APPEND ${PROJECT_NAME}Headers
		FlightGearComm.hpp
		flightGearUtils.hpp
		net_ctrls.hxx
		net_fdm.hxx
		)
	list(APPEND ${PROJECT_NAME}Libs
		${PLIB_LIBRARY} ${SIMGEAR_LIBRARIES} 
		)
endif()

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}Sources})
add_dependencies(${PROJECT_NAME} MAVLINK)
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}Libs})
set_target_properties(${PROJECT_NAME} PROPERTIES 
	VERSION ${LIBRARY_VERSION} 
	SOVERSION ${LIBRARY_SOVERSION})

install(TARGETS ${PROJECT_NAME} DESTINATION lib EXPORT ${PROJECT_NAME}-targets)
install(EXPORT ${PROJECT_NAME}-targets DESTINATION lib/${PROJECT_NAME})
install(FILES ${${PROJECT_NAME}Headers} DESTINATION include/${PROJECT_NAME})
