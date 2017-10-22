INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/modules/io/include")
ADD_DEFINITIONS (-DHAPLO_IO_MODULE)

ADD_LIBRARY (io SHARED
	block.c
	data.c
	decode.c
	encode.c
	entity.c
	error.c
	file.c
	fileslice.c
	filter.c
	gzindex.c
	gzio.c
	init.c
	io.c
	parser.c
	parser_UNV.c
	parser_UNV2411.c
	parser_UNV2412.c
	parser_UNV2414.c
	parser_UNV55.c
	parser_UNV56.c
	parser_UNV57.c
	parser_UNV58.c
	parser_UNVXX.c
	value.c
)

target_link_libraries (io haplo z)

ADD_CUSTOM_COMMAND (OUTPUT io.c
	COMMAND ${PERL_EXECUTABLE} ${PROJECT_SOURCE_DIR}/kernel/scripts/haplo-module.in 
	-e__io_init -f__io_fini -oio.c 
	-DHAPLO_IO_MODULE 
	-I${PROJECT_SOURCE_DIR}/modules/io/include 
	-I${CMAKE_BINARY_DIR}
	-I${PROJECT_SOURCE_DIR}/kernel/include
	-I${PROJECT_BINARY_DIR}/kernel/include
	${PROJECT_SOURCE_DIR}/modules/io/include/io.h
	DEPENDS ${PROJECT_SOURCE_DIR}/modules/io/include/io.h
	)

INSTALL (TARGETS io DESTINATION lib/haplo)
