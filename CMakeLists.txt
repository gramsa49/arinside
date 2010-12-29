CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(arinside)

#SET(CMAKE_VERBOSE_MAKEFILE 1)

IF (NOT DEFINED AR_INCLUDE_PATH)
  SET(AR_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../api750")
ENDIF(NOT DEFINED AR_INCLUDE_PATH)

IF(NOT EXISTS ${AR_INCLUDE_PATH})
  MESSAGE(FATAL_ERROR "ARAPI include directory not found! Please set the AR_INCLUDE_PATH variable to the api root, where the directories 'include' and 'lib' are located. Current directory: ${AR_INCLUDE_PATH}
use 'cmake -DAR_INCLUDE_PATH=/path/to/ars/api/dir'")
ENDIF(NOT EXISTS ${AR_INCLUDE_PATH})

if (NOT DEFINED ZLIB_INCLUDE_PATH)
  SET(ZLIB_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../libs/zlib")
ENDIF(NOT DEFINED ZLIB_INCLUDE_PATH)

INCLUDE_DIRECTORIES(${AR_INCLUDE_PATH}/include tinyxml/ tclap/ ./ ${ZLIB_INCLUDE_PATH}/include)

INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILES("strings.h" HAVE_STRINGS_H)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

ADD_DEFINITIONS(-DAR_STRICT_ANSI -D_CRT_SECURE_NO_DEPRECATE)
IF(UNIX)
 # use special compiler and linker flags on *nix platforms
 ADD_DEFINITIONS(-m32)
 SET(CMAKE_EXE_LINKER_FLAGS -m32)
ENDIF(UNIX)

SET (ARINSIDE_SRCS
 core/ARContainer.cpp core/ARGlobalField.cpp core/ARDataObject.cpp core/ARServerObject.cpp core/ARMessageObject.cpp
 core/ARObject.cpp core/ARDataFactory.cpp core/ARProplistHelper.cpp core/ARGroup.cpp core/ARAssignHelper.cpp
 core/ARDayStructHelper.cpp core/ARFilter.cpp core/ARVui.cpp core/AREscalation.cpp core/ARQualification.cpp
 core/ARServerInfoItem.cpp core/ARSchema.cpp core/ChangeHistoryEntry.cpp core/ARField.cpp core/ARUser.cpp
 core/ARImage.cpp core/ARCharMenu.cpp core/ARRole.cpp core/ARActiveLink.cpp core/ARValue.cpp core/ARServerInfo.cpp
 doc/DocFieldDetails.cpp doc/DocFilterActionStruct.cpp doc/DocAlActionStruct.cpp doc/DocBase.cpp 
 doc/DocUserDetails.cpp doc/DocVuiDetails.cpp doc/DocWebserviceDetails.cpp doc/DocMain.cpp
 doc/DocActionSetFieldsHelper.cpp doc/DocFilterGuideDetails.cpp doc/DocAlDetails.cpp
 doc/DocCharMenuDetails.cpp doc/DocContainerHelper.cpp doc/DocRoleDetails.cpp doc/DocAnalyzer.cpp
 doc/DocSchemaDetails.cpp doc/DocValidator.cpp doc/DocFilterDetails.cpp doc/DocImageDetails.cpp
 doc/DocActionOpenWindowHelper.cpp doc/DocPacklistDetails.cpp doc/DocApplicationDetails.cpp
 doc/DocGroupDetails.cpp doc/MessageItem.cpp doc/DocEscalationDetails.cpp doc/DocSummaryInfo.cpp
 doc/DocAlGuideDetails.cpp
 lists/ARFieldList.cpp lists/ARVUIList.cpp lists/ARContainerList.cpp lists/ARMenuList.cpp  lists/ARSchemaList.cpp
 lists/ARImageList.cpp lists/AREscalationList.cpp lists/ARActiveLinkList.cpp lists/BlackList.cpp
 lists/ARFilterList.cpp
 output/Table.cpp output/TableCell.cpp output/WebControl.cpp output/AlTable.cpp output/EscalTable.cpp
 output/UList.cpp output/FilterTable.cpp output/IFileStructure.cpp output/CsvPage.cpp output/ObjectTable.cpp
 output/FileNaming.cpp output/TableRow.cpp output/SchemaTable.cpp output/RoleTable.cpp output/ContainerTable.cpp
 output/WebPage.cpp output/TableColumn.cpp output/NavigationPage.cpp output/GroupTable.cpp output/ImageTable.cpp
 output/UListItem.cpp output/MenuTable.cpp output/UserTable.cpp output/WebUtil.cpp 
 tinyxml/tinyxml.cpp tinyxml/tinystr.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinyxmlparser.cpp
 util/ImageRefItem.cpp util/RefItem.cpp util/AppTimer.cpp util/Util.cpp util/MissingMenuRefItem.cpp
 util/BlackListItem.cpp
 AppConfig.cpp ARInside.cpp ConfigFile.cpp gzstream.cpp Main.cpp stdafx.cpp WindowsUtil.cpp
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
#SET (OBJCOPYOUTPUT elf32-i386)

# set link libraries for project
IF(UNIX)
  IF(NOT EXISTS ${CMAKE_OBJCOPY})
    MESSAGE(FATAL_ERROR "Could not find objcopy! Please make sure binutils are installed and the PATH is set correctly!")
  ENDIF(NOT EXISTS ${CMAKE_OBJCOPY})

  #LINK_LIBRARIES(${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS})
  LINK_LIBRARIES(${CMAKE_DL_LIBS})
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
    SET(OBJCOPYARCH sparc)
    SET(OBJCOPYOUTPUT elf32-sparc)
    LINK_LIBRARIES(nsl)
  else("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
    SET (OBJCOPYARCH i386)
    SET (OBJCOPYOUTPUT elf32-i386)
    ADD_DEFINITIONS(-malign-double)
  endif("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
ENDIF(UNIX)

IF(WIN32)
  SET(ARINSIDE_ARAPI_LIB arapi7603_build001 arapi75 arapi71 arapi70)
  SET(ZLIB_LIBNAMES zdll)
ELSE(WIN32)
  SET(ARINSIDE_ARAPI_LIB ar)
  SET(ZLIB_LIBNAMES z)
ENDIF(WIN32)

FIND_LIBRARY(ARS_API_LIB NAMES ${ARINSIDE_ARAPI_LIB} PATHS ${AR_INCLUDE_PATH}/lib)
FIND_LIBRARY(ZLIB_LIB NAMES ${ZLIB_LIBNAMES} PATHS ${ZLIB_INCLUDE_PATH}/lib)

IF (NOT ARS_API_LIB)
  MESSAGE(FATAL_ERROR "Could not find ${ARINSIDE_ARAPI_LIB} in '${AR_INCLUDE_PATH}/lib'! Please check or specify AR_INCLUDE_PATH variable")
ENDIF (NOT ARS_API_LIB)

IF (NOT ZLIB_LIB)
  MESSAGE(FATAL_ERROR "Could not find ${ZLIB_LIBNAMES} in '${ZLIB_INCLUDE_PATH}/lib'! Please check or specify ZLIB_INCLUDE_PATH variable")
ENDIF (NOT ZLIB_LIB)

# now import resources
IF(UNIX)
	LINK_DIRECTORIES(${arinside_BINARY_DIR}/res)
	FILE(MAKE_DIRECTORY ${arinside_BINARY_DIR}/res)
	FOREACH(res ${ARINSIDE_RESOURCES})
	  ADD_CUSTOM_COMMAND(OUTPUT "${res}.o"
					  COMMAND ${CMAKE_OBJCOPY} -I binary -B ${OBJCOPYARCH} -O ${OBJCOPYOUTPUT} "${res}" "${arinside_BINARY_DIR}/${res}.o"
					  DEPENDS "${res}"
					  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/"
					  COMMENT "Building ${arinside_BINARY_DIR}/${res}.o")
	  SET (ARINSIDE_RESOURCE_OBJECTS ${ARINSIDE_RESOURCE_OBJECTS} "${res}.o")
	ENDFOREACH(res)
ELSE(UNIX)
  SET (ARINSIDE_SRCS ${ARINSIDE_SRCS} "ARInside.rc")
ENDIF(UNIX)

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

ADD_EXECUTABLE(arinside ${ARINSIDE_SRCS} ${ARINSIDE_RESOURCE_OBJECTS})
TARGET_LINK_LIBRARIES(arinside ${ARS_API_LIB} ${ZLIB_LIB})

#INSTALL(TARGETS arinside DESTINATION bin)
