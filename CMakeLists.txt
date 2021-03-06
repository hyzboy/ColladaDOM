cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

####This is a hook for developers to use.
####Please have it print out what changes it makes via the CMake's message API.
include(developer.cmake OPTIONAL)

#SVN-EXTERNALS: cmake-modules/CMakePCHCompiler
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules/CMakePCHCompiler)

project(colladadom25 VERSION 2.5.0 LANGUAGES CXX CXXPCH)

#Note: No longer trying to hit C++98 in technical terms, because GCC will
#not accommodate the pointer-to-member templates in DAEP.h, that are core.
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#cmake-gui doesn't show this option. But be cautious since it may one day.
set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD} CACHE STRING "Must not be 98" FORCE)
mark_as_advanced(CLEAR CMAKE_CXX_STANDARD)
if(NOT CMAKE_CXX_STANDARD OR 98 EQUAL CMAKE_CXX_STANDARD)
	message("CMAKE_CXX_STANDARD is not set or 98. Set to 11")
	set(CMAKE_CXX_STANDARD 11)
endif()

#CMake should provide this level of abstraction, but it's dumb.
macro(_ f etc win)
	set(sel 0)
	if(WIN32)
		set(sel ${win})
	else()
		set(sel ${etc})
	endif()
	if(NOT sel EQUAL 0)
	set(_${f} ${sel})
	endif()
endmacro()
_(libpath -L -LIBPATH:)
_(whole-archive -Wl,--whole-archive 0)
_(no-whole-archive -Wl,--no-whole-archive 0)
_(no-strict-aliasing -fno-strict-aliasing 0) ##REMOVE ME
_(c++11 -std=gnu++11 0)
_(rtti -frtti 0)
_(visibility -fvisibility=hidden 0)
_(permissive "-fpermissive;-fno-delete-null-pointer-checks" 0)
_(no-delete-null-pointer-checks -fno-delete-null-pointer-checks 0)
_(Wall "-Wall;-Winvalid-pch" 0) ##How can -Wall be optional? (All warnings?)
_(Wno-multichar -Wno-multichar 0)
_(Wno-unused-but-set-variable "-Wno-unused-but-set-variable;-Wmaybe-uninitialized" 0)
#Windows flags (not necessarily MSVC.)
#These are set together in the subdirectories even though
#the compile options can be set anywhere, right here even.
#WARNING The / can be understood as a path by CMake. Dumb.
_(wincompile -mconsole /GF) #string-pooling 
_(winlink 0 /SUBSYSTEM:WINDOWS)
#O1 or O2 is needed to keep the file small to help it not outgrow the file system.
#Without g0 there is a "string table overflow at offset" and "File too big" error
#with GCC 6.3.0 on Cygwin. Removing debug makes it smaller. String pooling may be
#off in g0 mode also. It may be done in the linker.
_(winbigobj -Wa,-mbig-obj /bigobj)
_(gccbigobj "-g0;-O1" 0) #gcc is super slow.
if(NOT CYGWIN AND NOT WIN32)
	#error: unrecognized command line option -mconsole?
	set(_wincompile "") 
	#Apparently -mbig-obj is a Windows flag like -mconsole.
	set(_winbigobj "")
endif()

message(STATUS "Compiling COLLADA-DOM Version ${PROJECT_VERSION}")
message(STATUS "Using cmake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" )

#How many of these are used?
include(CheckIncludeFile) #Seem to need both this and CXX.
include(CheckIncludeFileCXX) #<unordered_map> feature test
#include(CheckLibraryExists)
#include(CheckFunctionExists)
#include(CheckCXXSourceCompiles)
#include(CheckCXXSourceRuns)
#include(CheckCXXCompilerFlag)
include(CheckTypeSize) #nullptr feature test
#include(PkgConfig)
find_package(PkgConfig QUIET) #Somehow enables pkg_check_modules???
if(PKGCONFIG_FOUND AND WIN32)
	#WHY NOT PKGCONFIG!?!?!?!?
	if(PKG_CONFIG_EXECUTABLE MATCHES "/bin/")
		message("Not using ${PKG_CONFIG_EXECUTABLE} because it looks like Cygwin (/bin/)")
		set(PKGCONFIG_FOUND 0)
	endif()
endif()
#Defines CMAKE_INSTALL_LIBDIR, but what else?
include(GNUInstallDirs)

message(STATUS "installing to ${CMAKE_INSTALL_PREFIX}")

####CMake-GUI options##########################

#These names need to stick out when building the external-libs projects.
option(COLLADA_DOM__LEGACY_LIBXML2 "Try to build-in LibXML2 I/O plugin" ON)
option(COLLADA_DOM__LEGACY_TINYXML "Try to build-in TinyXML I/O plugin" OFF)
option(COLLADA_DOM__COMPILE_VIEWER "Build colladadom3d reference/viewer" ON)
option(COLLADA_DOM__SINGLE_PRECISION "Use single-precision colladadom3d" ON)
option(COLLADA_DOM__TARGET_PS3 "Use its ancient PlayStation3 entrypoint" OFF)
option(COLLADA_DOM__ENABLE_LIBRARY_PCH "Use ColladaDOM.inl as PCH header" OFF)

####FEATURE TESTS##############################

#check_type_size doesn't know about CMAKE_CXX_STANDARD :(
if(NOT 98 EQUAL CMAKE_CXX_STANDARD) 
	list(APPEND CMAKE_REQUIRED_FLAGS ${_c++11})
endif()

#The new headers write nullptr instead of NULL or 0 but don't use
#its C++ template functionality. It's just for code style/reading.
check_type_size(nullptr HAVE_NULLPTR LANGUAGE CXX)
#98 was not required until CMAKE_CXX_STANDARD began to be used???
if(NOT HAVE_NULLPTR OR 98 EQUAL CMAKE_CXX_STANDARD)
	add_definitions(-DCOLLADA_DOM_NEED_NULLPTR)
endif()

#daePlatform.h needs this for preprocessor math and align pragmas.
#It uses SIZE_MAX by default, but it's a C++11 addition to limits.
if(8 EQUAL CMAKE_SIZEOF_VOID_P) 
	add_definitions(-DCOLLADA_UPTR_MAX=0xffFFffFFffFFffFFULL)
else()
	add_definitions(-DCOLLADA_UPTR_MAX=0xffFFffFF)
endif()

#map is used if this is not defined. Both use an allocator, and so
#it's debatable which is better at string and element name look up.
check_include_file_cxx("unordered_map" HAVE_UNORDERED_MAP)
#CMAKE_CXX_STANDARD is added because #include is standard agnostic.
if(HAVE_UNORDERED_MAP AND NOT 98 EQUAL CMAKE_CXX_STANDARD)
	add_definitions(-DCOLLADA_DOM_UNORDERED_MAP)
endif()

####BUILD/INSTALL###############################

#Annoying.
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -D_DEBUG")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -DNDEBUG")
string(APPEND CMAKE_CXX_FLAGS_MINSIZEREL " -DNDEBUG")
string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " -DNDEBUG")

add_compile_options(${_fno-strict-aliasing}) ##SCHEDULED FOR REMOVAL

add_compile_options(${_rtti} ${_visibility} ${_permissive} ${_Wall} ${_Wno-multichar})

macro(try_package name NAME)
	find_package(${name} QUIET) 
	if(NOT ${NAME}_FOUND AND PKGCONFIG_FOUND) 
		pkg_check_modules(${NAME} QUIET ${name})
	endif()
	if(${NAME}_FOUND) 		
		#standardize
		list(APPEND ${NAME}_LIBRARIES "${${NAME}_LIBRARY}")
		list(APPEND ${NAME}_INCLUDE_DIRS "${${NAME}_INCLUDE_DIR}")
		message(STATUS "${name} found: ${${NAME}_INCLUDE_DIRS}")
	else() 
		#Later versions of cmake set these to NOTFOUND and cry if they
		#are ever expanded :(
		set(${NAME}_LIBRARIES "") #unset
		set(${NAME}_INCLUDE_DIRS "") #unset
	endif()
endmacro()

#TODO: l should be the CMake target or??
macro(try_external_libs l name NAME)
	set(fullpath "${CMAKE_CURRENT_LIST_DIR}/external-libs/${name}")
	if(IS_DIRECTORY ${fullpath}) 
		message(STATUS "compiling ${name} from sources and linking statically")		
		add_subdirectory(external-libs/${name})
		list(APPEND ${NAME}_LIBRARIES ${l})
		set(${NAME}_INCLUDE_DIRS external-libs/${name})	
	else()
		message(STATUS "${fullpath} not found")
	endif()
endmacro()

include_directories(dom/include)

add_subdirectory(dom) #core component C++98 portable-library
if(COLLADA_DOM__COMPILE_VIEWER)
	message(STATUS "building ColladaDOM 3 Reference Viewer")

	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIRS})

	#daeDouble is expected to match btScalar for time being.
	try_package(bullet BULLET) 
	if(COLLADA_DOM__SINGLE_PRECISION AND BULLET_DEFINITIONS MATCHES "BT_USE_DOUBLE_PRECISION")
		set(COLLADA_DOM__SINGLE_PRECISION OFF)
		message(WARNING "COLLADA_DOM__SINGLE_PRECISION forced to match BT_USE_DOUBLE_PRECISION")				
	endif()
	if(COLLADA_DOM__SINGLE_PRECISION)
		add_definitions(-DCOLLADA_DOM_DOUBLE=float)
	endif()

	#find_package(Cg REQUIRED) #Cg (Nvidia)
	if(NOT DEFINED ENV{CG_INC_PATH})
	message(FATAL_ERROR "Did not detect Nvidia Cg SDK environment variable CG_INC_PATH")	
	endif()	
	#HACK Let Windows paths be used by Cygwin.
	#There are other ways but cmake_policy(SET CMP0021 OLD) seems to not work.
	#include_directories("$ENV{CG_INC_PATH}")
	add_definitions("-I\"$ENV{CG_INC_PATH}\"")

	add_subdirectory(fx)
	add_subdirectory(rt)	
	add_subdirectory(viewer) #colladadom3d (executable)
endif(COLLADA_DOM__COMPILE_VIEWER)

####PACKAGING###############################

set(bin ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
set(src ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME})

#This seems to be the new way to do this. Since CMake version??
#Note, the camel-case file names are part of the documentation.
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${bin}ConfigVersion.cmake"
	VERSION ${PROJECT_VERSION} 
	COMPATIBILITY SameMajorVersion)
configure_package_config_file("${PROJECT_NAME}Config.cmake.in" "${bin}Config.cmake"    
	INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}")
install(FILES 
	"${PROJECT_NAME}Config.cmake" 
	"${bin}ConfigVersion.cmake"
	DESTINATION "lib/cmake/${PROJECT_NAME}")
install(EXPORT "${PROJECT_NAME}Targets"
	NAMESPACE COLLADA_DOM::
	DESTINATION "lib/cmake/${PROJECT_NAME}")

#OBSOLETE? pkg-config support (I think)
#Note, dom/CMakeLists.txt sets up ${PC_LIBS} that is in turn expanded by colladadom25.pc.in.
configure_file("${src}.pc.in" "${bin}.pc" @ONLY IMMEDIATE)
install(FILES ${bin}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/ COMPONENT Devel)

#### Monkey see, monkey do??? ################

export(EXPORT ${PROJECT_NAME}Targets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/COLLADA_DOM/${PROJECT_NAME}Targets.cmake"
	NAMESPACE COLLADA_DOM::) #${PROJECT_NAME}

#############REMINDER#########################
#2017: There had been CPack code here. I don't see CPack in any CMakeLists.txt files today. It
#was a lot to maintain on the Debian specific front, including many details to keep up-to-date.
