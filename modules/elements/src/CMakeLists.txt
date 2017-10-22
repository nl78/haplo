INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/modules/elements/include")
INCLUDE_DIRECTORIES ("${PROJECT_SOURCE_DIR}/modules/fem/include")
INCLUDE_DIRECTORIES ("${CMAKE_CURRENT_BINARY_DIR}")

ADD_DEFINITIONS (-DHAPLO_ELEMENTS_MODULE)

SET (elm_list_c
	ME1DL2.c
	ME2DT3.c
	ME3DL2.c
)

ADD_LIBRARY (elements  SHARED
	elements.c
	feature.c	feature.h
	generic.c	generic.h
	init.c
	${elm_list_c}
)

TARGET_LINK_LIBRARIES (elements haplo fem)

ADD_CUSTOM_COMMAND (OUTPUT elements.c
	COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/kernel/scripts/haplo-module.in 
	-e__elements_init -f__elements_fini -oelements.c 
	-DHAPLO_ELEMENTS_MODULE 
	-I${PROJECT_SOURCE_DIR}/modules/elements/include 
	-I${PROJECT_SOURCE_DIR}/modules/fem/include 
	-I${CMAKE_BINARY_DIR}
	-I${PROJECT_SOURCE_DIR}/kernel/include
	-I${PROJECT_BINARY_DIR}/kernel/include
	${PROJECT_SOURCE_DIR}/modules/elements/include/elements.h
	DEPENDS ${PROJECT_SOURCE_DIR}/modules/elements/include/elements.h
	)

FOREACH (elm ${elm_list_c})
	STRING(REPLACE ".c" ".e" src ${elm}) 
	ADD_CUSTOM_COMMAND (OUTPUT ${elm}
		COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/modules/elements/src/gen-element.pl
		${CMAKE_CURRENT_SOURCE_DIR}/${src}
		${PROJECT_SOURCE_DIR}/modules/fem/include/fem
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${src})
ENDFOREACH (elm)

INSTALL (TARGETS elements DESTINATION lib/haplo)
