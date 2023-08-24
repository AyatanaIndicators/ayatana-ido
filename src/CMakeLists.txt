# ayatanamenuitemfactory.h
# idocalendarmenuitem.h
# idoentrymenuitem.h
# idorange.h
# idoscalemenuitem.h
# idoswitchmenuitem.h
# idotimeline.h
# libayatana-ido.h

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/ayatanamenuitemfactory.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libayatana-ido3-0.4/libayatana-ido")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/idocalendarmenuitem.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libayatana-ido3-0.4/libayatana-ido")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/idoentrymenuitem.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libayatana-ido3-0.4/libayatana-ido")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/idorange.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libayatana-ido3-0.4/libayatana-ido")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/idoscalemenuitem.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libayatana-ido3-0.4/libayatana-ido")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/idoswitchmenuitem.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libayatana-ido3-0.4/libayatana-ido")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/idotimeline.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libayatana-ido3-0.4/libayatana-ido")
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/libayatana-ido.h" DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libayatana-ido3-0.4/libayatana-ido")

set(HEADERS
    idorange.h
    idoappointmentmenuitem.h
    idousermenuitem.h
    idoalarmmenuitem.h
    idotimestampmenuitem.h
    idoplaybackmenuitem.h
    idoremovablemenuitem.h
    ayatanamenuitemfactory.h
    idobasicmenuitem.h
    idoapplicationmenuitem.h
    idotimeline.h
    idosourcemenuitem.h
    idoactionhelper.h
    idocalendarmenuitem.h
    idolocationmenuitem.h
    idoprogressmenuitem.h
    libayatana-ido.h
    idomediaplayermenuitem.h
    idoswitchmenuitem.h
    idoscalemenuitem.h
    idodetaillabel.h
    idoentrymenuitem.h
    idolevelmenuitem.h
)

set(SOURCES
    idotimestampmenuitem.c
    idodetaillabel.c
    libayatana-ido.c
    idomenuitemfactory.c
    idoactionhelper.c
    idoapplicationmenuitem.c
    idoplaybackmenuitem.c
    idomediaplayermenuitem.c
    idocalendarmenuitem.c
    ayatanamenuitemfactory.c
    idoprogressmenuitem.c
    idolocationmenuitem.c
    idoscalemenuitem.c
    idoentrymenuitem.c
    idoalarmmenuitem.c
    idorange.c
    idoappointmentmenuitem.c
    idoswitchmenuitem.c
    idoremovablemenuitem.c
    idobasicmenuitem.c
    idousermenuitem.c
    idosourcemenuitem.c
    idotimeline.c
    idolevelmenuitem.c
    ${CMAKE_CURRENT_BINARY_DIR}/idotypebuiltins.c
)

# idotypebuiltins.h

find_program(GLIB_MKENUMS glib-mkenums)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/idotypebuiltins.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    ${GLIB_MKENUMS}
    --template idotypebuiltins.h.template ${HEADERS}
    --output="${CMAKE_CURRENT_BINARY_DIR}/idotypebuiltins.h"
)

# idotypebuiltins.c

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/idotypebuiltins.c"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/idotypebuiltins.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    ${GLIB_MKENUMS}
    --template idotypebuiltins.c.template ${HEADERS}
    --output="${CMAKE_CURRENT_BINARY_DIR}/idotypebuiltins.c"
)

# ayatana-ido3-0.4.so

add_library("ayatana-ido3-0.4" SHARED ${SOURCES})
set_target_properties("ayatana-ido3-0.4" PROPERTIES VERSION 0.0.0 SOVERSION 0)
target_link_libraries("ayatana-ido3-0.4" ${PROJECT_DEPS_LIBRARIES} m)
target_include_directories("ayatana-ido3-0.4" PUBLIC ${PROJECT_DEPS_INCLUDE_DIRS})
target_include_directories("ayatana-ido3-0.4" PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories("ayatana-ido3-0.4" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions("ayatana-ido3-0.4" PUBLIC G_LOG_DOMAIN="IDO")
install(TARGETS "ayatana-ido3-0.4" LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}")

# AyatanaIdo3-0.4.gir

find_package(GObjectIntrospection REQUIRED QUIET)

if (INTROSPECTION_FOUND)

    set(HEADERS
        idocalendarmenuitem.h
        idoentrymenuitem.h
        idorange.h
        idoscalemenuitem.h
        idoswitchmenuitem.h
        idotimeline.h
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.gir"
        DEPENDS "ayatana-ido3-0.4"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND
        ${INTROSPECTION_SCANNER}
        ${SOURCES} ${HEADERS}
        --symbol-prefix=ido
        --identifier-prefix=Ido
        --namespace=AyatanaIdo3
        --nsversion=0.4
        --quiet
        --pkg=gtk+-3.0
        --warn-all
        --include=Gtk-3.0
        --c-include="libayatana-ido/libayatana-ido.h"
        --library="ayatana-ido3-0.4"
        --library-path=${CMAKE_CURRENT_BINARY_DIR}
        --output "${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.gir"
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.gir" DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/gir-1.0")

    # AyatanaIdo3-0.4.typelib

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.typelib"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.gir"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND
        ${INTROSPECTION_COMPILER}
        --includedir=${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.gir
        -o "${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.typelib"
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.typelib" DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/girepository-1.0")

    # libayatana-ido3-0.4.vapi

    find_package(Vala REQUIRED QUIET)

    if (VALA_FOUND)

        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libayatana-ido3-0.4.vapi"
            DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/AyatanaIdo3-0.4.typelib"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND
            ${VAPI_GEN}
            --library=libayatana-ido3-0.4
            --pkg gtk+-3.0
            AyatanaIdo3-0.4.gir
        )

        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libayatana-ido3-0.4.vapi" DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/vala/vapi")

        add_custom_target("src" ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/libayatana-ido3-0.4.vapi")

    endif ()

endif ()
