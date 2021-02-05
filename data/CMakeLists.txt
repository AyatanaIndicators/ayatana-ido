# Create the .pc

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/libayatana-ido3-0.4.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libayatana-ido3-0.4.pc")

# Install

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libayatana-ido3-0.4.pc" DESTINATION "${LIB_DIR_PATH}/pkgconfig/")
