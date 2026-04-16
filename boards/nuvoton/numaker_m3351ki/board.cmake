# SPDX-License-Identifier: Apache-2.0

if(CONFIG_BUILD_WITH_TFM)
  set_property(TARGET runners_yaml_props_target PROPERTY hex_file tfm_merged.hex)
endif()

board_runner_args(pyocd "--target=m3351kjcae")

include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
