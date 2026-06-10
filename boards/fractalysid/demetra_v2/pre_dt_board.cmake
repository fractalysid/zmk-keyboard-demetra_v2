# Copyright (c) 2026 Mattia Buscema
# SPDX-License-Identifier: MIT

# Sopprime il warning di unit-address duplicati (power/clock/acl/flash-controller),
# come fanno le board nRF52840 in-tree.
list(APPEND EXTRA_DTC_FLAGS "-Wno-unique_unit_address_if_enabled")
