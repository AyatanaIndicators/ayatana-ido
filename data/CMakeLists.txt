if (FLAVOUR_GTK4)
    set (ayatana_ido_gtkver "ayatana-ido4")
elseif (FLAVOUR_GTK3)
    set (ayatana_ido_gtkver "ayatana-ido3")
endif()

# libayatana-ido{3,4}-0.4.pc

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lib${ayatana_ido_gtkver}-0.4.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/lib${ayatana_ido_gtkver}-0.4.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/lib${ayatana_ido_gtkver}-0.4.pc" DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig/")
