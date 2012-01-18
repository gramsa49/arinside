###############################################################################
## Copyright (C) 2011 John Luthgers | jls17
##
## This file is part of ARInside.
##
##    ARInside is free software: you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation, version 2 of the License.
##
##    ARInside is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with ARInside.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(arinside)

#SET(CMAKE_VERBOSE_MAKEFILE 1)       # OR USE AT CMD-LINE: -DCMAKE_VERBOSE_MAKEFILE=1       ## for verbose makefiles
#SET(CMAKE_BUILD_TYPE Debug)         # OR USE AT CMD-LINE: -DCMAKE_BUILD_TYPE=Debug         ## for debug builds
#SET(ARINSIDE_TARGET_ARCHITECTURE 4) # OR USE AT CMD-LINE: -DARINSIDE_TARGET_ARCHITECTURE=4 ## for 32bit build on 64bit platform

MESSAGE(STATUS "System Name: ${CMAKE_SYSTEM_NAME}")

## *** taken from http://stackoverflow.com/questions/148570/using-pre-compiled-headers-with-cmake ***
## To use it, the PrecompiledSource file shouldn't be specified in SourceVar array. Otherwise the
## file specific precompiled properties will get overwritten.
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
	MESSAGE(STATUS "Setting up precompiler header support")
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
										   
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledBinary}\" /FI\"${PrecompiledBinary}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

# configurable variables for cmakegui
IF(NOT DEFINED ARINSIDE_ENABLE_ZLIB_SUPPORT)
	SET(ARINSIDE_ENABLE_ZLIB_SUPPORT CACHE BOOL "Enables zlib-compression support. See documentation for more details")
ENDIF(NOT DEFINED ARINSIDE_ENABLE_ZLIB_SUPPORT)

IF(NOT DEFINED ARINSIDE_BETA)
	SET(ARINSIDE_BETA CACHE BOOL "When enabled, appends 'Beta' to the version string.")
ENDIF(NOT DEFINED ARINSIDE_BETA)	

IF (NOT DEFINED AR_INCLUDE_PATH)
	SET(AR_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../api764" CACHE PATH "Path to the arapi root dir where 'include' and 'lib' is located.")
ENDIF(NOT DEFINED AR_INCLUDE_PATH)

IF(NOT EXISTS ${AR_INCLUDE_PATH})
  MESSAGE(FATAL_ERROR "ARAPI include directory not found! Please set the AR_INCLUDE_PATH variable to the api root, where the directories 'include' and 'lib' are located. Current directory: ${AR_INCLUDE_PATH}
use 'cmake -DAR_INCLUDE_PATH=/path/to/ars/api/dir'")
ENDIF(NOT EXISTS ${AR_INCLUDE_PATH})

if (NOT DEFINED ZLIB_INCLUDE_PATH)
  SET(ZLIB_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../libs/zlib" CACHE PATH "Path to the zlib directory. Only used if ARINSIDE_ENABLE_ZLIB_SUPPORT ist enabled.")
ENDIF(NOT DEFINED ZLIB_INCLUDE_PATH)

IF(NOT DEFINED ARINSIDE_TARGET_ARCHITECTURE)
	SET(ARINSIDE_TARGET_ARCHITECTURE ${CMAKE_SIZEOF_VOID_P})
ENDIF(NOT DEFINED ARINSIDE_TARGET_ARCHITECTURE)

INCLUDE_DIRECTORIES(${AR_INCLUDE_PATH}/include tinyxml/ tclap/ ./ ${ZLIB_INCLUDE_PATH}/include)

INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILES("strings.h" HAVE_STRINGS_H)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

ADD_DEFINITIONS(-DAR_STRICT_ANSI -D_CRT_SECURE_NO_DEPRECATE)
IF(ARINSIDE_ENABLE_ZLIB_SUPPORT)
	ADD_DEFINITIONS(-DARINSIDE_ENABLE_ZLIB_SUPPORT)
ENDIF(ARINSIDE_ENABLE_ZLIB_SUPPORT)
IF(ARINSIDE_BETA)
	ADD_DEFINITIONS(-D_ARINSIDE_BETA)
ENDIF(ARINSIDE_BETA)

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

MESSAGE(STATUS "Is GNU CXX? -- ${CMAKE_COMPILER_IS_GNUCXX}")
IF(UNIX AND CMAKE_COMPILER_IS_GNUCXX AND ARINSIDE_TARGET_ARCHITECTURE EQUAL 4 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	# use special compiler and linker flags on *nix platforms when doing 32bit cross-compile
	ADD_DEFINITIONS(-m32)
	SET(CMAKE_EXE_LINKER_FLAGS -m32)
ENDIF(UNIX AND CMAKE_COMPILER_IS_GNUCXX AND ARINSIDE_TARGET_ARCHITECTURE EQUAL 4 AND CMAKE_SIZEOF_VOID_P EQUAL 8)

SET (ARINSIDE_SRCS
 core/ARContainer.cpp core/ARGlobalField.cpp core/ARServerObject.cpp core/ARMessageObject.cpp
 core/ARObject.cpp core/ARProplistHelper.cpp core/ARGroup.cpp core/ARAssignHelper.cpp
 core/ARDayStructHelper.cpp core/ARFilter.cpp core/ARVui.cpp core/AREscalation.cpp core/ARQualification.cpp
 core/ARServerInfo.cpp core/ARSchema.cpp core/ChangeHistoryEntry.cpp core/ARField.cpp core/ARUser.cpp
 core/ARImage.cpp core/ARCharMenu.cpp core/ARRole.cpp core/ARActiveLink.cpp core/ARValue.cpp
 core/ARSetFieldHelper.cpp
 doc/DocFieldDetails.cpp doc/DocFilterActionStruct.cpp doc/DocAlActionStruct.cpp doc/DocBase.cpp 
 doc/DocUserDetails.cpp doc/DocVuiDetails.cpp doc/DocWebserviceDetails.cpp doc/DocMain.cpp
 doc/DocActionSetFieldsHelper.cpp doc/DocFilterGuideDetails.cpp doc/DocAlDetails.cpp
 doc/DocCharMenuDetails.cpp doc/DocContainerHelper.cpp doc/DocRoleDetails.cpp doc/DocAnalyzer.cpp
 doc/DocSchemaDetails.cpp doc/DocValidator.cpp doc/DocFilterDetails.cpp doc/DocImageDetails.cpp
 doc/DocActionOpenWindowHelper.cpp doc/DocPacklistDetails.cpp doc/DocApplicationDetails.cpp
 doc/DocGroupDetails.cpp doc/MessageItem.cpp doc/DocEscalationDetails.cpp doc/DocSummaryInfo.cpp
 doc/DocAlGuideDetails.cpp
 lists/ARActiveLinkList.cpp lists/ARContainerList.cpp lists/AREscalationList.cpp lists/ARFieldList.cpp
 lists/ARFilterList.cpp lists/ARGroupList.cpp lists/ARImageList.cpp lists/ARMenuList.cpp lists/ARRoleList.cpp
 lists/ARSchemaList.cpp lists/ARUserList.cpp lists/ARVUIList.cpp lists/BlackList.cpp lists/ARListHelpers.cpp
 lists/ARServerInfoList.cpp
 output/Table.cpp output/TableCell.cpp output/WebControl.cpp output/AlTable.cpp output/EscalTable.cpp
 output/UList.cpp output/FilterTable.cpp output/IFileStructure.cpp output/CsvPage.cpp output/ObjectTable.cpp
 output/FileNaming.cpp output/TableRow.cpp output/SchemaTable.cpp output/RoleTable.cpp output/ContainerTable.cpp
 output/WebPage.cpp output/TableColumn.cpp output/NavigationPage.cpp output/GroupTable.cpp output/ImageTable.cpp
 output/UListItem.cpp output/MenuTable.cpp output/UserTable.cpp output/WebUtil.cpp 
 scan/ScanActiveLinks.cpp scan/ScanContainers.cpp scan/ScanEscalations.cpp scan/ScanFields.cpp scan/ScanFilters.cpp
 scan/ScanMain.cpp scan/ScanSchema.cpp
 tinyxml/tinyxml.cpp tinyxml/tinystr.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinyxmlparser.cpp
 util/RefItem.cpp util/AppTimer.cpp util/Util.cpp util/BlackListItem.cpp
 AppConfig.cpp ARInside.cpp ConfigFile.cpp gzstream.cpp Main.cpp WindowsUtil.cpp
)
SET (ARINSIDE_SRC_GIFS
 res/active_link.gif res/al_guide.gif res/application.gif res/ars_edit.gif res/chapter.gif res/disabled.gif
 res/doc.gif res/down.gif res/edit.gif res/empty_list.gif res/escalation.gif res/filter.gif res/filter_guide.gif
 res/folder.gif res/group.gif res/hidden.gif res/image.gif res/info.gif res/menu.gif res/next.gif
 res/packing_list.gif res/prev.gif res/schema.gif res/search.gif res/server.gif res/sort_asc.gif res/sort_desc.gif
 res/up.gif res/user.gif res/visible.gif res/webservice.gif
)
SET (ARINSIDE_SRC_JS res/script.js res/sortscript.js res/tabscript.js)
SET (ARINSIDE_SRC_CSS res/style.css)
SET (ARINSIDE_RESOURCES ${ARINSIDE_SRC_GIFS} ${ARINSIDE_SRC_JS} ${ARINSIDE_SRC_CSS})

IF(NOT MSVC)
	SET (ARINSIDE_SRCS ${ARINSIDE_SRCS} stdafx.cpp)
ENDIF(NOT MSVC)

# set link libraries for project
IF(UNIX)
	FIND_PACKAGE(Threads)
	LINK_LIBRARIES(${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

	## solaris / aix specific libs
	IF(CMAKE_SYSTEM_NAME STREQUAL "SunOS" OR CMAKE_SYSTEM_NAME STREQUAL "AIX")
		LINK_LIBRARIES(nsl)
	ENDIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS" OR CMAKE_SYSTEM_NAME STREQUAL "AIX")

	IF(CMAKE_SYSTEM_NAME STREQUAL "AIX")
		LINK_LIBRARIES(tli)
	ENDIF(CMAKE_SYSTEM_NAME STREQUAL "AIX")
	
	IF(ARINSIDE_TARGET_ARCHITECTURE EQUAL 4 AND "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
		ADD_DEFINITIONS(-malign-double)
	ENDIF(ARINSIDE_TARGET_ARCHITECTURE EQUAL 4 AND "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
ENDIF(UNIX)

SET(ARINSIDE_ARAPI_ICUDATABMC_LIBNAME "icudatabmc")
SET(ARINSIDE_ARAPI_ICUI18NBMC_LIBNAME "icui18nbmc")
SET(ARINSIDE_ARAPI_ICUUCBMC_LIBNAME "icuucbmc")
IF(WIN32)
	SET(ARINSIDE_ARAPI_LIB arapi7604_build002 arapi7603_build001 arapi75 arapi71 arapi70)
	SET(ZLIB_LIBNAMES zlibwapi)
ELSE(WIN32)
	SET(ARINSIDE_ARAPI_LIB ar)
	SET(ZLIB_LIBNAMES z)
ENDIF(WIN32)

# set special library suffixes, compiler- and linker flags for 64-bit builds
IF(ARINSIDE_TARGET_ARCHITECTURE EQUAL 8)
	MESSAGE(STATUS "Building for 64 bit platform")
	SET(TEMP_ARAPI_LIBS "")
	
	IF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		SET(LIBAR_SUFFIX "_win64")
	ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		SET(LIBAR_SUFFIX "_lx64")
		IF(CMAKE_SIZEOF_VOID_P EQUAL 4) ## if we target 64-bit but the default is 32-bit, add some special flags
			ADD_DEFINITIONS(-m64)
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m64")
		ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
	ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
		SET(LIBAR_SUFFIX "_solsp64")
		IF(CMAKE_SIZEOF_VOID_P EQUAL 4) ## if we target 64-bit but the default is 32-bit, add some special flags
			ADD_DEFINITIONS(-m64)
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m64")
		ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
	ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "AIX")
		SET(LIBAR_SUFFIX "_aixp64")
		IF(CMAKE_SIZEOF_VOID_P EQUAL 4) ## if we target 64-bit but the default is 32-bit, add some special flags
			ADD_DEFINITIONS(-maix64 -mminimal-toc)
			SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-brtl -maix64")
		ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
	ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	
	FOREACH(TEMP_ARAPI_LIB_NAME ${ARINSIDE_ARAPI_LIB})
		LIST(APPEND TEMP_ARAPI_LIBS "${TEMP_ARAPI_LIB_NAME}${LIBAR_SUFFIX}")
	ENDFOREACH()
	SET(ARINSIDE_ARAPI_LIB ${TEMP_ARAPI_LIBS})
	
	SET(ARINSIDE_ARAPI_ICUDATABMC_LIBNAME "${ARINSIDE_ARAPI_ICUDATABMC_LIBNAME}${LIBAR_SUFFIX}")
	SET(ARINSIDE_ARAPI_ICUI18NBMC_LIBNAME "${ARINSIDE_ARAPI_ICUI18NBMC_LIBNAME}${LIBAR_SUFFIX}")
	SET(ARINSIDE_ARAPI_ICUUCBMC_LIBNAME "${ARINSIDE_ARAPI_ICUUCBMC_LIBNAME}${LIBAR_SUFFIX}")
	
	SET(ZLIB_LIB_PATH "${ZLIB_INCLUDE_PATH}/lib64")
ELSE(ARINSIDE_TARGET_ARCHITECTURE EQUAL 8)
	MESSAGE(STATUS "Building for 32 bit platform")
	SET(ZLIB_LIB_PATH "${ZLIB_INCLUDE_PATH}/lib")
ENDIF(ARINSIDE_TARGET_ARCHITECTURE EQUAL 8)

FIND_LIBRARY(ARS_API_LIB NAMES ${ARINSIDE_ARAPI_LIB} PATHS ${AR_INCLUDE_PATH}/lib)
FIND_LIBRARY(ARS_API_ICUDATABMC_LIB NAMES ${ARINSIDE_ARAPI_ICUDATABMC_LIBNAME} PATHS ${AR_INCLUDE_PATH}/lib)
FIND_LIBRARY(ARS_API_ICUI18NBMC_LIB NAMES ${ARINSIDE_ARAPI_ICUI18NBMC_LIBNAME} PATHS ${AR_INCLUDE_PATH}/lib)
FIND_LIBRARY(ARS_API_ICUUCBMC_LIB NAMES ${ARINSIDE_ARAPI_ICUUCBMC_LIBNAME} PATHS ${AR_INCLUDE_PATH}/lib)
FIND_LIBRARY(ZLIB_LIB NAMES ${ZLIB_LIBNAMES} PATHS ${ZLIB_LIB_PATH})

# check that necessary libs were found
MESSAGE(STATUS "libar: (${ARINSIDE_ARAPI_LIB}) -- ${ARS_API_LIB}")
IF (NOT ARS_API_LIB)
	MESSAGE(FATAL_ERROR "Could not find ${ARINSIDE_ARAPI_LIB} in '${AR_INCLUDE_PATH}'! Please check or specify AR_INCLUDE_PATH variable")
ENDIF (NOT ARS_API_LIB)

IF (ARINSIDE_ENABLE_ZLIB_SUPPORT AND NOT ZLIB_LIB)
	MESSAGE(FATAL_ERROR "Could not find ${ZLIB_LIBNAMES} in '${ZLIB_LIB_PATH}'! Please check or specify ZLIB_INCLUDE_PATH variable")
ENDIF (ARINSIDE_ENABLE_ZLIB_SUPPORT AND NOT ZLIB_LIB)

MESSAGE(STATUS "icudatabmc: (${ARINSIDE_ARAPI_ICUDATABMC_LIBNAME}) -- ${ARS_API_ICUDATABMC_LIB}")
IF(ARS_API_ICUDATABMC_LIB)
	LINK_LIBRARIES(${ARS_API_ICUDATABMC_LIB})
ENDIF(ARS_API_ICUDATABMC_LIB)

MESSAGE(STATUS "icui18nbmc: (${ARINSIDE_ARAPI_ICUI18NBMC_LIBNAME}) -- ${ARS_API_ICUI18NBMC_LIB}")
IF(ARS_API_ICUI18NBMC_LIB)
	LINK_LIBRARIES(${ARS_API_ICUI18NBMC_LIB})
ENDIF(ARS_API_ICUI18NBMC_LIB)

MESSAGE(STATUS "icuucbmc: (${ARINSIDE_ARAPI_ICUUCBMC_LIBNAME}) -- ${ARS_API_ICUUCBMC_LIB}")
IF(ARS_API_ICUUCBMC_LIB)
	LINK_LIBRARIES(${ARS_API_ICUUCBMC_LIB})
ENDIF(ARS_API_ICUUCBMC_LIB)

# now generate and import resources
IF(UNIX)
	IF(NOT DEFINED ARINSIDE_RESOURCES_ALREADY_BUILD)
		INCLUDE_DIRECTORIES(${arinside_BINARY_DIR}/res)
		FILE(MAKE_DIRECTORY ${arinside_BINARY_DIR}/res)
		SET(ARINSIDE_GENERATED_RESOURCE_SRC_FILES "")
		FOREACH(res ${ARINSIDE_RESOURCES})
			MESSAGE(STATUS "Generating ${res}.cpp ...")
			FILE(READ ${res} ARINSIDE_RES_OBJECT HEX)
			STRING(LENGTH ${ARINSIDE_RES_OBJECT} ARINSIDE_RES_OBJECT_LENGTH)
			SET(ARINSIDE_RES_OBJECT_CONTENT "")
			SET(ARINSIDE_RES_OBJECT_POSITION 0)
			WHILE(${ARINSIDE_RES_OBJECT_POSITION} LESS ${ARINSIDE_RES_OBJECT_LENGTH})
				STRING(SUBSTRING ${ARINSIDE_RES_OBJECT} ${ARINSIDE_RES_OBJECT_POSITION} 2 RES_CHAR)
				MATH(EXPR ARINSIDE_RES_OBJECT_POSITION "${ARINSIDE_RES_OBJECT_POSITION}+2")

				IF(NOT ${ARINSIDE_RES_OBJECT_CONTENT} STREQUAL "")
					SET(ARINSIDE_RES_OBJECT_CONTENT "${ARINSIDE_RES_OBJECT_CONTENT},")
				ENDIF(NOT ${ARINSIDE_RES_OBJECT_CONTENT} STREQUAL "")

				SET(ARINSIDE_RES_OBJECT_CONTENT "${ARINSIDE_RES_OBJECT_CONTENT}0x${RES_CHAR}")
			ENDWHILE(${ARINSIDE_RES_OBJECT_POSITION} LESS ${ARINSIDE_RES_OBJECT_LENGTH})
			
			STRING(REGEX REPLACE "[^A-Za-z0-9]" "_" RES_OBJECT_NAME "${res}")
			SET(RES_OBJECT_NAME "_binary_${RES_OBJECT_NAME}")
			MATH(EXPR ARINSIDE_RES_REAL_OBJECT_LENGTH "${ARINSIDE_RES_OBJECT_LENGTH}/2")
			SET(ARINSIDE_RES_OBJECT_CONTENT "unsigned char ${RES_OBJECT_NAME}_start[] = {${ARINSIDE_RES_OBJECT_CONTENT}};\nunsigned int ${RES_OBJECT_NAME}_size = ${ARINSIDE_RES_REAL_OBJECT_LENGTH};\n\n")
			
			SET(RES_OUTPUT_NAME "${arinside_BINARY_DIR}/${res}.cpp")
			FILE(WRITE ${RES_OUTPUT_NAME} "${ARINSIDE_RES_OBJECT_CONTENT}")
			
			LIST(APPEND ARINSIDE_GENERATED_RESOURCE_SRC_FILES ${res}.cpp)
		ENDFOREACH(res)
		SET(ARINSIDE_RESOURCES_ALREADY_BUILD 1 CACHE INTERNAL "to remember resources are already build")
		SET(ARINSIDE_GENERATED_RESOURCE_SRC_FILES ${ARINSIDE_GENERATED_RESOURCE_SRC_FILES} CACHE INTERNAL "to remember build source files")
	ENDIF(NOT DEFINED ARINSIDE_RESOURCES_ALREADY_BUILD)
	SET(ARINSIDE_SRCS ${ARINSIDE_SRCS} ${ARINSIDE_GENERATED_RESOURCE_SRC_FILES})
ELSE(UNIX)
	SET(ARINSIDE_SRCS ${ARINSIDE_SRCS} "ARInside.rc")
ENDIF(UNIX)

ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" ARINSIDE_SRCS)

# generate svnrev.h file
FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
	Subversion_WC_INFO(${PROJECT_SOURCE_DIR} ARINSIDE)
ELSE(Subversion_FOUND)
	SET(ARINSIDE_WC_REVISION SVNMISS)
ENDIF(Subversion_FOUND)
FILE(READ ${PROJECT_SOURCE_DIR}/svnrev_template.h SVNREV_TEMPLATE)
STRING(REPLACE "$WCREV$" ${ARINSIDE_WC_REVISION} SVNREV_TEMPLATE "${SVNREV_TEMPLATE}")
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/svnrev.h ${SVNREV_TEMPLATE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# setup output target
ADD_EXECUTABLE(arinside ${ARINSIDE_SRCS} ${ARINSIDE_RESOURCE_OBJECTS})
TARGET_LINK_LIBRARIES(arinside ${ARS_API_LIB} ${ZLIB_LIB})

#INSTALL(TARGETS arinside DESTINATION bin)
