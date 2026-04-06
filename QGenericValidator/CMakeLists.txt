cmake_minimum_required(VERSION 3.19)
project(QGenericValidator LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(QGenericValidator
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindowpanel.cpp
    mainwindowpanel.h
    mainwindowpanel.ui
    ProjectDocument.md
    ValidatorTraits.h
    ValidationResult.h
    validationManager.h validationManager.cpp
)

target_link_libraries(QGenericValidator
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS QGenericValidator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QGenericValidator
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
