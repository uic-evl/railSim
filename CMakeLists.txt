if(NOT OMEGA_BUILD_OSG_LIB)
	message("omega osg library not available, railSimmodeshapes will not be built")
else()
	add_executable(railSimmodeshapes 
		railSimmodeshapes.cpp)

	set_target_properties(railSimmodeshapes PROPERTIES FOLDER apps)

	target_link_libraries(railSimmodeshapes
		omega 
		omegaToolkit
		omegaOsg
		cyclops)
endif()