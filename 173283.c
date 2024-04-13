void bnx2x_update_mfw_dump(struct bnx2x *bp)
{
	u32 drv_ver;
	u32 valid_dump;

	if (!SHMEM2_HAS(bp, drv_info))
		return;

	/* Update Driver load time, possibly broken in y2038 */
	SHMEM2_WR(bp, drv_info.epoc, (u32)ktime_get_real_seconds());

	drv_ver = bnx2x_update_mng_version_utility(DRV_MODULE_VERSION, true);
	SHMEM2_WR(bp, drv_info.drv_ver, drv_ver);

	SHMEM2_WR(bp, drv_info.fw_ver, REG_RD(bp, XSEM_REG_PRAM));

	/* Check & notify On-Chip dump. */
	valid_dump = SHMEM2_RD(bp, drv_info.valid_dump);

	if (valid_dump & FIRST_DUMP_VALID)
		DP(NETIF_MSG_IFUP, "A valid On-Chip MFW dump found on 1st partition\n");

	if (valid_dump & SECOND_DUMP_VALID)
		DP(NETIF_MSG_IFUP, "A valid On-Chip MFW dump found on 2nd partition\n");
}