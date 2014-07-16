###############################################################################
## ARInsideLib source files
SET (ARINSIDE_SRCS_CORE_CPP
 core/ARActiveLink.cpp
 core/ARAssignHelper.cpp
 core/ARCharMenu.cpp
 core/ARContainer.cpp
 core/ARDayStructHelper.cpp
 core/AREnum.cpp
 core/AREscalation.cpp
 core/ARField.cpp
 core/ARFilter.cpp
 core/ARGlobalField.cpp
 core/ARGroup.cpp
 core/ARHandle.cpp
 core/ARImage.cpp
 core/ARObject.cpp
 core/ARParseField.cpp
 core/ARProplistHelper.cpp
 core/ARQualification.cpp
 core/ARRole.cpp
 core/ARSchema.cpp
 core/ARServerInfo.cpp
 core/ARServerObject.cpp
 core/ARSetFieldHelper.cpp
 core/ARStatusList.cpp
 core/ARUser.cpp
 core/ARValue.cpp
 core/ARVui.cpp
 core/ChangeHistoryEntry.cpp
)
SET (ARINSIDE_SRCS_CORE_H
 core/ARActiveLink.h
 core/ARAssignHelper.h
 core/ARCharMenu.h
 core/ARContainer.h
 core/ARDayStructHelper.h
 core/AREnum.h
 core/AREscalation.h
 core/ARField.h
 core/ARFilter.h
 core/ARGlobalField.h
 core/ARGroup.h
 core/ARHandle.h
 core/ARImage.h
 core/ARObject.h
 core/ARParseField.h
 core/ARProplistHelper.h
 core/ARQualification.h
 core/ARRole.h
 core/ARSchema.h
 core/ARServerInfo.h
 core/ARServerObject.h
 core/ARSetFieldHelper.h
 core/ARStatusList.h
 core/ARUser.h
 core/ARValue.h
 core/ARVui.h
 core/ChangeHistoryEntry.h
)
SOURCE_GROUP(core FILES ${ARINSIDE_SRCS_CORE_CPP} ${ARINSIDE_SRCS_CORE_H})

SET(ARINSIDE_SRCS_DOC_CPP
 doc/DocActionOpenWindowHelper.cpp
 doc/DocActionSetFieldsHelper.cpp
 doc/DocAlActionStruct.cpp
 doc/DocAlDetails.cpp
 doc/DocAlGuideDetails.cpp
 doc/DocAllMatchingIdsTable.cpp
 doc/DocAnalyzer.cpp
 doc/DocBase.cpp
 doc/DocCharMenuDetails.cpp
 doc/DocContainerHelper.cpp
 doc/DocCurrencyField.cpp
 doc/DocCustomWorkflow.cpp
 doc/DocEscalationDetails.cpp
 doc/DocFieldDetails.cpp
 doc/DocFilterActionStruct.cpp
 doc/DocFilterDetails.cpp
 doc/DocFilterGuideDetails.cpp
 doc/DocGroupDetails.cpp
 doc/DocImageDetails.cpp
 doc/DocImageOverview.cpp
 doc/DocMain.cpp
 doc/DocOverlayHelper.cpp
 doc/DocPacklistDetails.cpp
 doc/DocApplicationDetails.cpp
 doc/DocRoleDetails.cpp
 doc/DocSchemaDetails.cpp
 doc/DocStatusHistoryField.cpp
 doc/DocSummaryInfo.cpp
 doc/DocTextReferences.cpp
 doc/DocUserDetails.cpp
 doc/DocValidator.cpp
 doc/DocVuiDetails.cpp
 doc/DocWebserviceDetails.cpp
 doc/MessageItem.cpp
)
SET(ARINSIDE_SRCS_DOC_H
 doc/DocActionOpenWindowHelper.h
 doc/DocActionSetFieldsHelper.h
 doc/DocAlActionStruct.h
 doc/DocAlDetails.h
 doc/DocAlGuideDetails.h
 doc/DocAllMatchingIdsTable.h
 doc/DocAnalyzer.h
 doc/DocApplicationDetails.h
 doc/DocBase.h
 doc/DocCharMenuDetails.h
 doc/DocContainerHelper.h
 doc/DocCurrencyField.h
 doc/DocCustomWorkflow.h
 doc/DocEscalationDetails.h
 doc/DocFieldDetails.h
 doc/DocFilterActionStruct.h
 doc/DocFilterDetails.h
 doc/DocFilterGuideDetails.h
 doc/DocGroupDetails.h
 doc/DocImageDetails.h
 doc/DocImageOverview.h
 doc/DocMain.h
 doc/DocOverlayHelper.h
 doc/DocPacklistDetails.h
 doc/DocRoleDetails.h
 doc/DocSchemaDetails.h
 doc/DocStatusHistoryField.h
 doc/DocSummaryInfo.h
 doc/DocTextReferences.h 
 doc/DocUserDetails.h
 doc/DocValidator.h
 doc/DocVuiDetails.h
 doc/DocWebserviceDetails.h
 doc/MessageItem.h
)
SOURCE_GROUP(doc FILES ${ARINSIDE_SRCS_DOC_CPP} ${ARINSIDE_SRCS_DOC_H})

SET(ARINSIDE_SRCS_LISTS_CPP
 lists/ARActiveLinkList.cpp
 lists/ARContainerList.cpp
 lists/AREscalationList.cpp
 lists/ARFieldList.cpp
 lists/ARFilterList.cpp
 lists/ARGroupList.cpp
 lists/ARImageList.cpp
 lists/ARListHelpers.cpp
 lists/ARMenuList.cpp
 lists/ARRoleList.cpp
 lists/ARSchemaList.cpp
 lists/ARServerInfoList.cpp
 lists/ARUserList.cpp
 lists/ARVUIList.cpp
 lists/BlackList.cpp
)
SET(ARINSIDE_SRCS_LISTS_H
 lists/ARActiveLinkList.h
 lists/ARContainerList.h
 lists/AREscalationList.h
 lists/ARFieldList.h
 lists/ARFilterList.h
 lists/ARGroupList.h
 lists/ARImageList.h
 lists/ARListHelpers.h
 lists/ARMenuList.h
 lists/ARRoleList.h
 lists/ARSchemaList.h
 lists/ARServerInfoList.h
 lists/ARUserList.h
 lists/ARVUIList.h
 lists/BlackList.h
)
SOURCE_GROUP(lists FILES ${ARINSIDE_SRCS_LISTS_CPP} ${ARINSIDE_SRCS_LISTS_H})

SET(ARINSIDE_SRCS_LISTS_SUPPORT_CPP
 lists/support/SchemaDbQueryBuilder.cpp
)
SET(ARINSIDE_SRCS_LISTS_SUPPORT_H
 lists/support/SchemaDbQueryBuilder.h
)
SOURCE_GROUP(lists\\support FILES ${ARINSIDE_SRCS_LISTS_SUPPORT_CPP} ${ARINSIDE_SRCS_LISTS_SUPPORT_H})

SET(ARINSIDE_SRCS_OUTPUT_CPP
 output/AlTable.cpp
 output/ContainerTable.cpp
 output/CsvPage.cpp
 output/EscalTable.cpp
 output/FileNaming.cpp
 output/FilterTable.cpp
 output/GroupTable.cpp
 output/IFileStructure.cpp
 output/ImageTable.cpp
 output/ImageTag.cpp
 output/LetterFilterControl.cpp
 output/MenuTable.cpp
 output/NavigationPage.cpp
 output/ObjectTable.cpp
 output/ObjNotFound.cpp
 output/RoleTable.cpp
 output/RootPath.cpp
 output/SchemaTable.cpp
 output/TabControl.cpp
 output/Table.cpp
 output/TableCell.cpp
 output/TableColumn.cpp
 output/TableRow.cpp
 output/UList.cpp
 output/UListItem.cpp
 output/URLLink.cpp
 output/UserTable.cpp
 output/WebControl.cpp
 output/WebPage.cpp
 output/WebUtil.cpp
 output/WorkflowReferenceTable.cpp
)
SET(ARINSIDE_SRCS_OUTPUT_H
 output/AlTable.h
 output/ContainerTable.h
 output/CsvPage.h
 output/EscalTable.h
 output/FileNaming.h
 output/FilterTable.h
 output/GroupTable.h
 output/IFileStructure.h
 output/ImageTable.h
 output/ImageTag.h
 output/LetterFilterControl.h
 output/MenuTable.h
 output/NavigationPage.h 
 output/ObjectTable.h
 output/ObjNotFound.h
 output/RoleTable.h
 output/RootPath.h
 output/SchemaTable.h
 output/TabControl.h
 output/Table.h
 output/TableCell.h
 output/TableColumn.h
 output/TableRow.h
 output/UList.h
 output/UListItem.h
 output/URLLink.h
 output/UserTable.h
 output/WebControl.h
 output/WebPage.h
 output/WebUtil.h
 output/WorkflowReferenceTable.h
)
SOURCE_GROUP(output FILES ${ARINSIDE_SRCS_OUTPUT_CPP} ${ARINSIDE_SRCS_OUTPUT_H})

SET(ARINSIDE_SRCS_OUTPUT_WEBPAGE_CPP
 output/webpage/HtmlReferenceList.cpp
 output/webpage/HtmlReferenceListImpl.cpp
)
SET(ARINSIDE_SRCS_OUTPUT_WEBPAGE_H
 output/webpage/HtmlReferenceList.h
 output/webpage/HtmlReferenceListImpl.h
)
SOURCE_GROUP(output\\webpage FILES ${ARINSIDE_SRCS_OUTPUT_WEBPAGE_CPP} ${ARINSIDE_SRCS_OUTPUT_WEBPAGE_H})

SET(ARINSIDE_SRCS_SCAN_CPP
 scan/ScanActiveLinks.cpp
 scan/ScanContainers.cpp
 scan/ScanEscalations.cpp
 scan/ScanFields.cpp
 scan/ScanFilters.cpp
 scan/ScanImages.cpp
 scan/ScanMain.cpp
 scan/ScanMenus.cpp
 scan/ScanSchema.cpp
)
SET(ARINSIDE_SRCS_SCAN_H
 scan/ScanActiveLinks.h
 scan/ScanContainers.h
 scan/ScanEscalations.h
 scan/ScanFields.h
 scan/ScanFilters.h
 scan/ScanImages.h
 scan/ScanMain.h
 scan/ScanMenus.h
 scan/ScanSchema.h
)
SOURCE_GROUP(scan FILES ${ARINSIDE_SRCS_SCAN_CPP} ${ARINSIDE_SRCS_SCAN_H})

SET(ARINSIDE_SRCS_THIRDPARTY_TINYXML_CPP
 thirdparty/tinyxml/tinystr.cpp
 thirdparty/tinyxml/tinyxml.cpp
 thirdparty/tinyxml/tinyxmlerror.cpp 
 thirdparty/tinyxml/tinyxmlparser.cpp
)
SOURCE_GROUP(thirdparty\\tinyxml FILES ${ARINSIDE_SRCS_THIRDPARTY_TINYXML_CPP})

SET(ARINSIDE_SRCS_UTIL_CPP
 util/AppTimer.cpp
 util/BlackListItem.cpp
 util/CommandLineValidator.cpp
 util/RefItem.cpp 
 util/ResourceFileLocatorAndExtractor.cpp
 util/UntarStream.cpp
 util/Util.cpp 
)
SET(ARINSIDE_SRCS_UTIL_H
 util/AppTimer.h
 util/BlackListItem.h
 util/CommandLineValidator.h
 util/RefItem.h
 util/ResourceFileLocatorAndExtractor.h
 util/Uncopyable.h
 util/UntarStream.h
 util/Util.h
)
SOURCE_GROUP(util FILES ${ARINSIDE_SRCS_UTIL_CPP} ${ARINSIDE_SRCS_UTIL_H})

SET(ARINSIDE_SRCS_CPP
 ARInside.cpp
 AppConfig.cpp
 AppConfigReader.cpp
 ConfigFile.cpp
 FileSystemUtil.cpp
 Main.cpp
 gzstream.cpp
 stdafx.cpp
)

SET(ARINSIDE_SRCS_H
 ARApi.h
 ARInside.h
 AppConfig.h
 AppConfigReader.h
 AppException.h
 ConfigFile.h
 FileSystemUtil.h
 Main.h
 gzstream.h
 stdafx.h
 svnrev.h
)

### all source code files
SET(ARINSIDE_SRCS_CPP
 ${ARINSIDE_SRCS_CORE_CPP}
 ${ARINSIDE_SRCS_DOC_CPP}
 ${ARINSIDE_SRCS_LISTS_CPP}
 ${ARINSIDE_SRCS_LISTS_SUPPORT_CPP}
 ${ARINSIDE_SRCS_OUTPUT_CPP}
 ${ARINSIDE_SRCS_OUTPUT_WEBPAGE_CPP}
 ${ARINSIDE_SRCS_SCAN_CPP}
 ${ARINSIDE_SRCS_THIRDPARTY_TINYXML_CPP}
 ${ARINSIDE_SRCS_UTIL_CPP}
 ${ARINSIDE_SRCS_CPP}
)
### all header files/dependencies
SET(ARINSIDE_SRCS_H
 ${ARINSIDE_SRCS_CORE_H}
 ${ARINSIDE_SRCS_DOC_H}
 ${ARINSIDE_SRCS_LISTS_H}
 ${ARINSIDE_SRCS_LISTS_SUPPORT_H}
 ${ARINSIDE_SRCS_OUTPUT_H}
 ${ARINSIDE_SRCS_OUTPUT_WEBPAGE_H}
 ${ARINSIDE_SRCS_SCAN_H}
 ${ARINSIDE_SRCS_UTIL_H}
 ${ARINSIDE_SRCS_H}
)

SET(ARINSIDE_SRCS ${ARINSIDE_SRCS_CPP} ${ARINSIDE_SRCS_H})
ADD_MSVC_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" ARINSIDE_SRCS_CPP)

###############################################################################
## ARInside executable source files
SET(ARINSIDE_MAIN_SRCS
 ARInsideMain.cpp
 ARInsideMain.h
)

###############################################################################
## ARInsideTest source files
SET(ARINSIDE_TEST_SRCS
 ARInsideTest.cpp
 ARInsideTest.h
 AppConfigReaderTest.cpp
 FileSystemUtilTest.cpp
 FileSystemUtilTest.h
 RapidJSONTests.cpp
 core/ARDayStructHelperTest.cpp
 core/ARParseFieldTest.cpp
 doc/DocTextReferencesTest.cpp
 doc/DocTextReferencesTest.h
 lists/ARListHelpersTest.cpp
 lists/support/SchemaDbQueryBuilderTest.cpp
 output/webpage/HtmlReferenceListImplTest.cpp
 output/ImageTagTest.cpp
 output/ObjNotFoundTest.cpp
 output/URLLinkTest.cpp
 test/fakes/FakeARServerObject.cpp
 test/fakes/FakeARServerObject.h
 util/CommandLineValidatorTest.cpp
 util/RefItemTest.cpp
 util/UntarStreamTest.cpp
 util/UntarStreamTest.h
)
