project(account-manager)
cmake_minimum_required(VERSION 3.5.1)


if( PROJECT MATCHES "bi" OR PROJECT MATCHES "telstra")
    set(PROJECT "telstra")

endif()
message("Project name : ${PROJECT}")

# Disable zero check for visual studio
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_VERBOSE_MAKEFILE true)

# Disable auto rpath and set -Wl,-rpath,. later
set(CMAKE_SKIP_BUILD_RPATH true)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

if(WIN32)
    set(PLATFORM "win-x86_64")
else()
    set(PLATFORM "linux-x86_64")
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/outputs")
#set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/outputs)
#set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/outputs)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/outputs)
# if(MSVC)
#     set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RUNTIME_OUTPUT_DIRECTORY}")
#     set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RUNTIME_OUTPUT_DIRECTORY}")
#     set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${LIBRARY_OUTPUT_DIRECTORY}")
#     set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${LIBRARY_OUTPUT_DIRECTORY}")
# endif()


##
# Complier options
#
if(MSVC)
    #use boost static link
    set(BOOST_STATIC true)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE TYPE INTERNAL FORCE)
    #for winsock2.h windows.h conflict
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    # sdk win7+
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-DDLL_IMPORTS)
    add_definitions(-DBOOST_ALL_NO_LIB) #disable auto linking
    # Disable deprecate warning
    add_compile_options(/wd4996)
    # Disable type conversion warning
    add_compile_options(/wd4244)
    add_compile_options(/wd4267)
    add_compile_options(/wd4503)
else()
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

##
# C++11
#
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    # MSVC, On by default (if available)
endif()

##
# POSIX Thread
#
if(MSVC)
    #do nothing
elseif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 3.1.0)
    message("cmake 3.1.0+")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    set(LIBS ${LIBS} Threads::Threads)
elseif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.8.11)
    message("cmake 2.8.12+")
    find_package(Threads REQUIRED)
    if(THREADS_HAVE_PTHREAD_ARG)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    endif()
    if(CMAKE_THREAD_LIBS_INIT)
        set(LIBS ${LIBS} "${CMAKE_THREAD_LIBS_INIT}")
    endif()
else()
    message("Not supported cmake version")
endif()

set(EXTERNALS_ROOT ${CMAKE_SOURCE_DIR}/import)
set(PREBUILTS_ROOT ${CMAKE_SOURCE_DIR}/external)

##
# Dpendencies
#
# LibDL
set(LIBS ${LIBS} ${CMAKE_DL_LIBS})

# Boost
if(BOOST_STATIC)
    set(BOOST_ALL_DYN_LINK       OFF)
    set(Boost_USE_STATIC_LIBS    ON)
    set(Boost_USE_MULTITHREADED  ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
else()
    set(BOOST_ALL_DYN_LINK       OFF)
    set(Boost_USE_STATIC_LIBS    OFF)
    set(Boost_USE_MULTITHREADED  ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    add_definitions(-DBOOST_LOG_DYN_LINK)
endif()
set(Boost_NO_SYSTEM_PATHS ON)
set(BOOST_ROOT ${PREBUILTS_ROOT}/boost)
set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)
set(BOOST_LIBRARYDIR ${BOOST_ROOT}/${PLATFORM}/lib)
if(WIN32)
    find_package(Boost REQUIRED COMPONENTS
        atomic chrono coroutine context date_time filesystem iostreams locale
        log log_setup program_options random regex system thread zlib
    )
else()
    find_package(Boost REQUIRED COMPONENTS
        atomic chrono coroutine context date_time filesystem iostreams locale
	log log_setup program_options random regex system thread
    )
endif()

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

# Gorilla HTTP
link_directories(${EXTERNALS_ROOT}/gorilla/http/lib)
include_directories(${EXTERNALS_ROOT}/gorilla/http/include)

# curl 
if(WIN32)
    set(CURL_ROOT "${PREBUILTS_ROOT}/curl")
    set(CURL_INCLUDE_DIR "${CURL_ROOT}/include")
    set(CURL_LIBRARY  "${CURL_ROOT}/${PLATFORM}/lib")
    set(OPENSSL_ROOT_DIR "${PREBUILTS_ROOT}/openssl")
    SET(OPENSSL_LIBRARIES_DIR "${OPENSSL_ROOT_DIR}/${PLATFORM}/lib")
    SET(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")

    include_directories(${CURL_INCLUDE_DIR})
    link_directories(${CURL_LIBRARY})
    include_directories(${OPENSSL_INCLUDE_DIR})
    link_directories(${OPENSSL_LIBRARIES_DIR})
else()
    set(CURL_ROOT "${PREBUILTS_ROOT}/curl")
    set(CURL_INCLUDE_DIR "${CURL_ROOT}/include")
    set(CURL_LIBRARY  "${CURL_ROOT}/${PLATFORM}/lib")
	
    include_directories(${CURL_INCLUDE_DIR})
    link_directories(${CURL_LIBRARY})
	
    set(LIBS ${LIBS} gorilla_http) #MUST before boost
endif()

#openldap
if(UNIX)
    set(OPENLDAP_ROOT "${PREBUILTS_ROOT}/openldap")
	set(OPENLDAP_INCLUDE_DIR "${PREBUILTS_ROOT}/openldap/include")
	set(OPENLDAP_LIBRARIES_DIR "${PREBUILTS_ROOT}/openldap/${PLATFORM}/lib")
	include_directories(${OPENLDAP_INCLUDE_DIR})
	link_directories(${OPENLDAP_LIBRARIES_DIR})
    set(OPENLDAP_2_4_9_ROOT_DIR "${PREBUILTS_ROOT}/openldap/${PLATFORM}/openldap-2.4.49/openldapc")
    set(OPENLDAP_2_4_9_INCLUDE_DIR "${OPENLDAP_2_4_9_ROOT_DIR}")
    set(OPENLDAP_2_4_9_LIBRARIES_DIR "${OPENLDAP_2_4_9_ROOT_DIR}/libraries")
    include_directories(${OPENLDAP_2_4_9__INCLUDE_DIR})
    link_directories(${OPENLDAP_2_4_9_LIBRARIES_DIR})
	set(LIBS ${LIBS} lber ldap ldap_r ldapcpp rewrite)
endif()


#sasl2
#if(UNIX)
#    set(SASL2_ROOT "${PREBUILTS_ROOT}/sasl2")
#	set(SASL2_INCLUDE_DIR "${PREBUILTS_ROOT}/sasl2/include")
#	set(SASL2_LIBRARIES_DIR "${PREBUILTS_ROOT}/sasl2/${PLATFORM}/lib")
#	include_directories(${SASL2_INCLUDE_DIR})
#	link_directories(${SASL2_LIBRARIES_DIR})
#	set(LIBS ${LIBS} sasl2)
#endif()

##### Externals

#gorilla utility 
include_directories(${EXTERNALS_ROOT}/gorilla/utility)
file(GLOB SRCS_GORILLA_UTILITY
   ${EXTERNALS_ROOT}/gorilla/utility/*.cpp
)
set(SRCS ${SRCS} ${SRCS_GORILLA_UTILITY})

#gorilla log
include_directories(${EXTERNALS_ROOT}/gorilla/log)
file(GLOB SRCS_GORILLA_LOG
   ${EXTERNALS_ROOT}/gorilla/log/*.cpp
)
set(SRCS ${SRCS} ${SRCS_GORILLA_LOG})

# hash
include_directories(${EXTERNALS_ROOT}/hash)
#set(SRCS ${SRCS} ${EXTERNAL_ROOT}/hash/hash.c)
file(GLOB SRCS_HASH
   ${EXTERNALS_ROOT}/hash/*.c
)
set(SRCS ${SRCS} ${SRCS_HASH})

# SQLite
include_directories(${EXTERNALS_ROOT}/sqlite3)
set(SRCS ${SRCS} ${EXTERNALS_ROOT}/sqlite3/sqlite3.c)

if(MSVC)
    include_directories(${EXTERNALS_ROOT}/syslog)
    set(SRCS ${SRCS} ${EXTERNALS_ROOT}/syslog/syslog.c)
endif()

#cppnetlib
include_directories(${EXTERNALS_ROOT})
include_directories(${EXTERNALS_ROOT}/cppnetlib/include)

#jsoncpp
include_directories(${EXTERNALS_ROOT}/jsoncpp)
set(SRCS ${SRCS} ${EXTERNALS_ROOT}/jsoncpp/jsoncpp.cpp)

#json-develop
include_directories(${EXTERNALS_ROOT}/json-develop)

add_definitions(-DBOOST_NETWORK_NO_LIB)
add_definitions(-DBOOST_NETWORK_ENABLE_HTTPS)

# src 
include_directories(${CMAKE_SOURCE_DIR})
file(GLOB SRCS_PROJ
    ${CMAKE_SOURCE_DIR}/*.cpp
)
set(SRCS ${SRCS} ${SRCS_PROJ})

##
# Exectable file
include(generate_product_version)
generate_product_version(
    VersionFilesOutputVariable
    NAME "account-manager"
    ICON "${CMAKE_SOURCE_DIR}/cmake/account-manager.ico"
    VERSION_MAJOR $ENV{VERSION_MAJOR}
    VERSION_MINOR $ENV{VERSION_MINOR}
    VERSION_PATCH $ENV{VERSION_PATCH}
    VERSION_REVISION $ENV{VERSION_REVISION}

)
add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} ${LIBS})

##
# Linking
#   #IVARSocketPack
if(MSVC)
    set(LIBS ${LIBS} libcurl libeay32 ssleay32 bcrypt Wldap32)
    target_link_libraries(${PROJECT_NAME} debug HttpFrameworkD optimized HttpFramework)
    target_link_libraries(${PROJECT_NAME} ${LIBS})
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGCC_PLATFORM")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGCC_PLATFORM")
    set(LIBS ${LIBS} crypto ssl rt curl sqlite3 z )
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-Wl,-rpath,.")
    target_link_libraries(${PROJECT_NAME} -lstdc++ ${LIBS})
endif()

##
# Installation
#
# Executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
    ARCHIVE DESTINATION .)

# Prebuilt libraries
#install(DIRECTORY
#    ${PREBUILTS_ROOT}/IVARSocketPack/${PLATFORM}/lib/
#    DESTINATION .
#    USE_SOURCE_PERMISSIONS)
if(WIN32)
    install(DIRECTORY
        ${PREBUILTS_ROOT}/curl/${PLATFORM}/bin/
        ${PREBUILTS_ROOT}/openssl/${PLATFORM}/bin/
        DESTINATION .)
    #install(DIRECTORY
    #    ${PREBUILTS_ROOT}/${PLATFORM}/cmd/
    #   DESTINATION ./cmd)
endif()

set(CONFIG_PATH "${CMAKE_SOURCE_DIR}/configs")
# Configurations
install(FILES
    ${CONFIG_PATH}/common/dh2048.pem
	${CONFIG_PATH}/common/ldap_config.json
	${CONFIG_PATH}/common/server.pem
	${CONFIG_PATH}/common/settings.ini
    DESTINATION .)

#install(DIRECTORY
#        ${CONFIG_PATH}/default
#        DESTINATION .)
if(PROJECT)
	set(CONFIG_PATH "${CONFIG_PATH}/${PROJECT}")
	message("Config Path ${CONFIG_PATH}")
	install(FILES
		${CONFIG_PATH}/config.json
		DESTINATION .)
else()
	install(FILES
		${CONFIG_PATH}/common/config.json
		DESTINATION .)
endif()

if(WIN32)
    install(DIRECTORY
        ${CMAKE_SOURCE_DIR}/packaging/servicewrapper/
        DESTINATION .)
elseif(UNIX)
    install(DIRECTORY
        ${CMAKE_SOURCE_DIR}/packaging/systemd
        DESTINATION .
        USE_SOURCE_PERMISSIONS)
endif()
