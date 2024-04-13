static void bnx2x__common_init_phy(struct bnx2x *bp)
{
	u32 shmem_base[2], shmem2_base[2];
	/* Avoid common init in case MFW supports LFA */
	if (SHMEM2_RD(bp, size) >
	    (u32)offsetof(struct shmem2_region, lfa_host_addr[BP_PORT(bp)]))
		return;
	shmem_base[0] =  bp->common.shmem_base;
	shmem2_base[0] = bp->common.shmem2_base;
	if (!CHIP_IS_E1x(bp)) {
		shmem_base[1] =
			SHMEM2_RD(bp, other_shmem_base_addr);
		shmem2_base[1] =
			SHMEM2_RD(bp, other_shmem2_base_addr);
	}
	bnx2x_acquire_phy_lock(bp);
	bnx2x_common_init_phy(bp, shmem_base, shmem2_base,
			      bp->common.chip_id);
	bnx2x_release_phy_lock(bp);
}