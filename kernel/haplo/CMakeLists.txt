ADD_EXECUTABLE (haplo-exe 
	main.c
	hgetopt.c	hgetopt.h
	${PROJECT_BINARY_DIR}/kernel/include/haplo/config.h
)

TARGET_LINK_LIBRARIES (haplo-exe haplo)

SET_TARGET_PROPERTIES(haplo-exe
	PROPERTIES OUTPUT_NAME haplo
)

INSTALL (TARGETS haplo-exe DESTINATION bin)
