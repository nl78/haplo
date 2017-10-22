INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/modules/net/include")

ADD_DEFINITIONS (-DHAPLO_NET_MODULE)

ADD_LIBRARY (net SHARED
	client.c
	dns.c
	identity.c
	server.c
)

TARGET_LINK_LIBRARIES (net haplo)

INSTALL (TARGETS net DESTINATION lib/haplo)