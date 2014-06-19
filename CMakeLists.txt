request_dependency(cyclops)
request_dependency(skyboxes)

include_directories(${OSG_INCLUDES})
add_executable(railSimmodeshapes 
		railSimmodeshapes.cpp)
set_target_properties(railSimmodeshapes PROPERTIES FOLDER apps)

target_link_libraries(railSimmodeshapes
	omega 
	omegaToolkit
	omegaOsg
	cyclops)
