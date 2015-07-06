
# Prevent compilation in-source
if( ${CMAKE_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR} )
  Message( " " )
  Message( FATAL_ERROR "Source and build  directories are the same.
 Create an empty build directory,
 change into it and re-invoke cmake")
endif()

#Path of all custom header files
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/headers)


#    Tell CMake where to leave executables
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
IF(APPLE)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/macosx/)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/macosx/)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/macosx/)
ELSEIF(UNIX)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/linux/)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/linux/)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/linux/)
ELSEIF(WIN32)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/win32/)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/win32/)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/win32/)
ENDIF()



# Tell CMake where to find the externally compiled libraries
if(APPLE)
link_directories(${PROJECT_SOURCE_DIR}/extern/macosx/)
ELSEIF(UNIX)
link_directories(${PROJECT_SOURCE_DIR}/extern/CentOS_x86_64/)
ELSEIF(WIN32)
link_directories(${PROJECT_SOURCE_DIR}/extern/Win_x86_64/)
ENDIF()

# ### Search Additional Libraries ##########
SET( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )
MESSAGE(${CMAKE_SOURCE_DIR})



FIND_PACKAGE(CXX11)
#FIND_PACKAGE(OpenCV)
FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

##if not unix then we will have to introduce the no unistd option in the lexer...thats what the next if achieves
##remember that the performance of the lexer under windows is significantly impacted due to lack of standard unix types but works nonetheless

if(UNIX)
configure_file(${PROJECT_SOURCE_DIR}/configuration/RPcustomKW18lexer.l_unix ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18lexer.l)
else(WIN32)
configure_file(${PROJECT_SOURCE_DIR}/configuration/RPcustomKW18lexer.l_win32 ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18lexer.l)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#if you do not have flex and bison installed then the slower nonunistd version will be used to parse files
#I have assumed that if you do not have Bison and Flex then you are on a non *nix system and hence this parser will be slower..only way to ensure full compatibility of types
if(BISON_FOUND AND FLEX_FOUND)
BISON_TARGET(KW18Parser RPcustomKW18parser.y ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18parser.cxx)
FLEX_TARGET(KW18Scanner ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18lexer.l ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18lexer.cxx)
ADD_FLEX_BISON_DEPENDENCY(KW18Scanner KW18Parser)
SET(MU_SOURCE ${MU_SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18lexer.l RPcustomKW18parser.y)
else()
MESSAGE("FLEX and BISON paths must be set to compile the system optimized parser..else the slower pre-compiled headers will be used")
configure_file(${PROJECT_SOURCE_DIR}/configuration/RPcustomKW18lexer.cxx_config.in ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18lexer.cxx)
configure_file(${PROJECT_SOURCE_DIR}/configuration/RPcustomKW18parser.hxx_config.in ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18parser.hxx)
configure_file(${PROJECT_SOURCE_DIR}/configuration/RPcustomKW18parser.cxx_config.in ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18parser.cxx)
endif()



if(NOT WIN32)
  FIND_PACKAGE(X11)
  FIND_PACKAGE(Threads REQUIRED)
endif()

# #### End of additional libraries search ##########


### Configure Paths according to detected packages




if (OpenCV_FOUND)
  message("OpenCV Found")
  include_directories(${OpenCV_INCLUDE_DIRS})
  link_directories(${OpenCV_LIB_DIRS})
  SET( SYSTEM_LIBS  ${SYSTEM_LIBS}  ${OpenCV_LIBS} )
endif()



#Enable C++11 support 
if(CXX11_FOUND)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")
endif()

if(CMAKE_COMPILER_IS_GNUCC)
  # needed for gcc 4.6+
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
endif()



SET(ENABLE_TILED_TIFF_READER OFF CACHE BOOL "TILED TIFF READER")

if(ENABLE_TILED_TIFF_READER)
MESSAGE("TILED TIFF READER ENABLED..TIFF library is a strict requirement")
FIND_PACKAGE(TIFF REQUIRED)
if(TIFF_FOUND)
  get_filename_component(TIFF_LIB_DIRS ${TIFF_LIBRARIES} PATH)
  link_directories(${TIFF_LIB_DIRS})
  include_directories(${TIFF_INCLUDE_DIR})
  SET(SYSTEM_LIBS ${SYSTEM_LIBS} ${TIFF_LIBRARIES})
endif()
SET(MU_SOURCE ${MU_SOURCE} TiffReader.cxx)
configure_file(${PROJECT_SOURCE_DIR}/configuration/tiff_reader_config.h.in tiff_reader_config.h)
else()
configure_file(${PROJECT_SOURCE_DIR}/configuration/tiff_reader_config_alt.h.in tiff_reader_config.h)
endif()

#SET(MU_HEADERS ${PROJECT_SOURCE_DIR}/headers/trackerdriver.hpp ${PROJECT_SOURCE_DIR}/headers/utilz.hpp ${PROJECT_SOURCE_DIR}/headers/target_track.hpp)
#SET(MU_SOURCE ${MU_SOURCE} trackerdriver.cxx utilz.cxx filelisting_RP.cxx nonpssread_.cxx)

SET(MU_SOURCE ${MU_SOURCE} main.cxx)

if(BISON_FOUND AND FLEX_FOUND)
add_executable (flex_bison_parsing ${BISON_KW18Parser_OUTPUTS} ${FLEX_KW18Scanner_OUTPUTS} ${MU_SOURCE} )
else()
add_executable(flex_bison_parsing ${MU_SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18lexer.cxx ${CMAKE_CURRENT_BINARY_DIR}/RPcustomKW18parser.cxx)
endif()
target_link_libraries(flex_bison_parsing ${SYSTEM_LIBS} )



