# Install Targets for yaml-cpp
install(TARGETS yaml-cpp EXPORT PDFxTMDLibTargets)

# Install PDFxTMDLib library
install(TARGETS PDFxTMDLib_static PDFxTMDLib_shared
        EXPORT PDFxTMDLibTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install header files
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/PDFxTMDLib)

# Export targets for package config
install(EXPORT PDFxTMDLibTargets
        FILE PDFxTMDLibTargets.cmake
        NAMESPACE PDFxTMDLib::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PDFxTMDLib)

# Configure package config file using the provided PDFxTMDLibConfig.cmake.in
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/PDFxTMDLibConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/PDFxTMDLibConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PDFxTMDLib
)

# Install the generated config file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PDFxTMDLibConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PDFxTMDLib)
