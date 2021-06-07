# libayatana-ido3-0.4.pc

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libayatana-ido3-0.4.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libayatana-ido3-0.4.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libayatana-ido3-0.4.pc" DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig/")
