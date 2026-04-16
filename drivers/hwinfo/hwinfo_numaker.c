/*
 * Copyright (c) 2024 Nuvoton Technology Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/sys/byteorder.h>
#include <string.h>

/* For TrustZone Non-Secure, switch to NSC version */
#if defined(CONFIG_ARM_NONSECURE_FIRMWARE)
#include <tfm_platform_hal_ioctl_api.h>
#define NVT_SECURE_CALL(FUNC) NVT_TFM_PLAT_IOCTL_NS(FUNC)
#else
#define NVT_SECURE_CALL(FUNC) FUNC
#endif

struct numaker_uid {
	uint32_t id[3];
};

ssize_t z_impl_hwinfo_get_device_id(uint8_t *buffer, size_t length)
{
	struct numaker_uid dev_id;
	bool was_reg_locked = (SYS->REGLCTL == 0);
	bool was_fmc_opened = (FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk);

	if (was_reg_locked) {
		NVT_SECURE_CALL(SYS_UnlockReg)();
	}
	if (!was_fmc_opened) {
		NVT_SECURE_CALL(FMC_Open)();
	}

	dev_id.id[0] = sys_cpu_to_be32(FMC_ReadUID(0));
	dev_id.id[1] = sys_cpu_to_be32(FMC_ReadUID(1));
	dev_id.id[2] = sys_cpu_to_be32(FMC_ReadUID(2));

	length = MIN(length, sizeof(dev_id.id));
	memcpy(buffer, dev_id.id, length);

	if (!was_fmc_opened) {
		NVT_SECURE_CALL(FMC_Close)();
	}
	if (was_reg_locked) {
		NVT_SECURE_CALL(SYS_LockReg)();
	}

	return length;
}
