# Copyright (c) 2026 Mattia Buscema
# SPDX-License-Identifier: MIT

if(CONFIG_DEMETRA_V2_BOOT_MARKER OR CONFIG_DEMETRA_V2_STORAGE_NUKE)
  zephyr_library()
  zephyr_library_sources_ifdef(CONFIG_DEMETRA_V2_BOOT_MARKER boot_marker.c)
  zephyr_library_sources_ifdef(CONFIG_DEMETRA_V2_STORAGE_NUKE storage_nuke.c)
endif()
