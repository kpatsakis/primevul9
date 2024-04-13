void bnx2x_update_mng_version(struct bnx2x *bp)
{
	u32 iscsiver = DRV_VER_NOT_LOADED;
	u32 fcoever = DRV_VER_NOT_LOADED;
	u32 ethver = DRV_VER_NOT_LOADED;
	int idx = BP_FW_MB_IDX(bp);
	u8 *version;

	if (!SHMEM2_HAS(bp, func_os_drv_ver))
		return;

	mutex_lock(&bp->drv_info_mutex);
	/* Must not proceed when `bnx2x_handle_drv_info_req' is feasible */
	if (bp->drv_info_mng_owner)
		goto out;

	if (bp->state != BNX2X_STATE_OPEN)
		goto out;

	/* Parse ethernet driver version */
	ethver = bnx2x_update_mng_version_utility(DRV_MODULE_VERSION, true);
	if (!CNIC_LOADED(bp))
		goto out;

	/* Try getting storage driver version via cnic */
	memset(&bp->slowpath->drv_info_to_mcp, 0,
	       sizeof(union drv_info_to_mcp));
	bnx2x_drv_info_iscsi_stat(bp);
	version = bp->slowpath->drv_info_to_mcp.iscsi_stat.version;
	iscsiver = bnx2x_update_mng_version_utility(version, false);

	memset(&bp->slowpath->drv_info_to_mcp, 0,
	       sizeof(union drv_info_to_mcp));
	bnx2x_drv_info_fcoe_stat(bp);
	version = bp->slowpath->drv_info_to_mcp.fcoe_stat.version;
	fcoever = bnx2x_update_mng_version_utility(version, false);

out:
	SHMEM2_WR(bp, func_os_drv_ver[idx].versions[DRV_PERS_ETHERNET], ethver);
	SHMEM2_WR(bp, func_os_drv_ver[idx].versions[DRV_PERS_ISCSI], iscsiver);
	SHMEM2_WR(bp, func_os_drv_ver[idx].versions[DRV_PERS_FCOE], fcoever);

	mutex_unlock(&bp->drv_info_mutex);

	DP(BNX2X_MSG_MCP, "Setting driver version: ETH [%08x] iSCSI [%08x] FCoE [%08x]\n",
	   ethver, iscsiver, fcoever);
}