# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ARM_NONSECURE_FIRMWARE)
  # BSP relies on NVT_CMSE_NON_SECURE for Non-Secure
  zephyr_compile_definitions(NVT_CMSE_NON_SECURE)
endif()
