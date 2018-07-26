#include_directories(../include)

add_executable(mredit_example
  main.cpp
  MainWindow.cpp
  MainWindow.h

  resources/resources.qrc
)

target_link_libraries(mredit_example 
	Qt5::Widgets 
	mredit::editor
)