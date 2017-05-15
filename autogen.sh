#!/bin/sh

PKG_NAME="ido"

which mate-autogen || {
    echo "You need mate-common from https://git.mate-desktop.org/mate-common"
    exit 1
}

USE_GNOME2_MACROS=1 \
USE_COMMON_DOC_BUILD=yes \
mate-autogen --enable-gtk-doc $@
