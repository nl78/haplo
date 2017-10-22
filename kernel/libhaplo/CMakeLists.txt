INCLUDE (FindBISON)
BISON_TARGET(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/_parser.c)
INCLUDE_DIRECTORIES ("${CMAKE_CURRENT_SOURCE_DIR}")

ADD_LIBRARY (haplo SHARED 
	bt.c	   	  bt.h
	builtin.c	  builtin.h
	code.c		  code.h
	${PROJECT_BINARY_DIR}/kernel/include/haplo/config.h
	des.c		  des.h
	error.c		  error.h
	func.c		  func.h
	hash.c		  hash.h
	hash_str.c	  hash_str.h
	hash_ul.c	  hash_ul.h
	help.c		  help.h
	init.c
	interruptible.c	  interruptible.h
	list.c		  list.h
	memory.c	  memory.h
	module.c	  module.h
	object.c	  object.h
	olist.c		  olist.h
	parser.y	  parser.h
	pool.c		  pool.h
	slink.c		  slink.h
	timer.c		  timer.h
	utils.c		  utils.h
	${BISON_PARSER_OUTPUT_SOURCE}
)
TARGET_LINK_LIBRARIES (haplo ${HAPLO_MODULE_LIB})

INSTALL (TARGETS haplo DESTINATION lib)
