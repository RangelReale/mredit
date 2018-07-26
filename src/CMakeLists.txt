include_directories(../include)

add_library(mredit STATIC
  Editor.cpp
  Label.cpp
  TextDocument.cpp
  PlainTextDocumentLayout.cpp
  TextBlockUserData.cpp
  BookmarkGroup.cpp
  margin/MarginStacker.cpp
  margin/AbstractMargin.cpp
  margin/LineBookmarkMargin.cpp
  margin/LineNumberMargin.cpp
  margin/LineRevisionMargin.cpp
  margin/LineSpacingMargin.cpp

  ../include/mredit/Global.h
  ../include/mredit/Editor.h
  ../include/mredit/Label.h
  ../include/mredit/TextDocument.h
  ../include/mredit/PlainTextDocumentLayout.h
  ../include/mredit/TextBlockUserData.h
  ../include/mredit/BookmarkGroup.h
  ../include/mredit/margin/MarginStacker.h
  ../include/mredit/margin/AbstractMargin.h
  ../include/mredit/margin/LineBookmarkMargin.h
  ../include/mredit/margin/LineNumberMargin.h
  ../include/mredit/margin/LineRevisionMargin.h
  ../include/mredit/margin/LineSpacingMargin.h
)

set_target_properties(mredit PROPERTIES DEBUG_POSTFIX d)

# set this target's include directories
target_include_directories(mredit PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:../include>
    PRIVATE src)

target_link_libraries(mredit 
	Qt5::Widgets 
)

# export
include(GNUInstallDirs)
install(TARGETS mredit 
    EXPORT mredit-export
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT mredit-export
    FILE mreditTargets.cmake
    NAMESPACE mredit::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mredit
)

add_library(mredit::editor ALIAS mredit)

# IDE source groups
FILE(GLOB SG_ROOT ../include/mredit/* *.cpp)
source_group(src FILES ${SG_ROOT})
source_group(src\\margin REGULAR_EXPRESSION margin/.*)
