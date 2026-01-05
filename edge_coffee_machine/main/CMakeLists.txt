idf_component_register(SRCS main.c 
                       LDFRAGMENTS "../components/Qul/linker.lf"
                       PRIV_REQUIRES Qul)

set_target_properties(${COMPONENT_LIB} PROPERTIES CXX_STANDARD 14 CXX_EXTENSIONS ON C_EXTENSIONS ON)