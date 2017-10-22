INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/modules/fem/include")
ADD_DEFINITIONS (-DHAPLO_FEM_MODULE)

	
ADD_LIBRARY (fem SHARED
	analysis.c	analysis.h
	bc.c		bc.h
	cara.c		cara.h
	determinant.c	determinant.h
	dof.c		dof.h
	eigen.c		eigen.h
	element.c	element.h
	fem.c
	geom.c		geom.h
	group.c		group.h
	hook.c		hook.h
	init.c
	integration.c	integration.h
	load.c		load.h
	material.c	material.h
	matrix.c	matrix.h
	meshing.c	meshing.h
	model.c		model.h
	numbering.c	numbering.h
	post.c		post.h
	problem.c	problem.h
	property.c	property.h
	renum.c		renum.h
	solver.c	solver.h
	vectorized.c	vectorized.h
)
TARGET_LINK_LIBRARIES (fem haplo)


ADD_CUSTOM_COMMAND (OUTPUT fem.c
	COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/kernel/scripts/haplo-module.in 
	-e__fem_init -f__fem_fini -ofem.c 
	-DHAPLO_FEM_MODULE 
	-I${PROJECT_SOURCE_DIR}/modules/fem/include 
	-I${CMAKE_BINARY_DIR}
	-I${PROJECT_SOURCE_DIR}/kernel/include
	-I${PROJECT_BINARY_DIR}/kernel/include
	${PROJECT_SOURCE_DIR}/modules/fem/include/fem.h
	DEPENDS ${PROJECT_SOURCE_DIR}/modules/fem/include/fem.h
	)

INSTALL (TARGETS fem DESTINATION lib/haplo)
