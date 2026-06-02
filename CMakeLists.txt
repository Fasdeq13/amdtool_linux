cmake_minimum_required(VERSION 3.16)
project(amdtool_linux VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Core Gui REQUIRED)

set(SOURCES
        main.cpp
)

add_executable(amdtool_linux ${SOURCES})

target_include_directories(amdtool_linux PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_library(AMDSMI_LIB amd_smi)
if(AMDSMI_LIB)
    target_link_libraries(amdtool_linux PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui ${AMDSMI_LIB})
else()
    target_link_libraries(amdtool_linux PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)
endif()


include(GNUInstallDirs)

install(TARGETS amdtool_linux RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES logo.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps/ RENAME amdtool_linux.svg)

file(WRITE ${CMAKE_BINARY_DIR}/amdtool_linux.desktop
        "[Desktop Entry]\n"
        "Name=AMD Tool Adrenalin\n"
        "Comment=Universal graphical control panel suite for AMD GPU\n"
        "Exec=amdtool_linux\n"
        "Icon=amdtool_linux\n"
        "Terminal=false\n"
        "Type=Application\n"
        "Categories=Settings;System;\n"
)
install(FILES ${CMAKE_BINARY_DIR}/amdtool_linux.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)


set(CPACK_PACKAGE_NAME "amdtool-linux")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "Fasdeq13")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Universal GUI manager clone of AMD Adrenalin App for Linux systems")
set(CPACK_PACKAGE_CONTACT "Fasdeq13 <https://github.com>")

set(CPACK_GENERATOR "DEB;RPM;TGZ")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6, libqt6core6, libqt6gui6")

set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase-gui, qt6-qtbase")

include(CPack)
