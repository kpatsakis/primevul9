static int bnx2x_init_shmem(struct bnx2x *bp)
{
	int cnt = 0;
	u32 val = 0;

	do {
		bp->common.shmem_base = REG_RD(bp, MISC_REG_SHARED_MEM_ADDR);

		/* If we read all 0xFFs, means we are in PCI error state and
		 * should bail out to avoid crashes on adapter's FW reads.
		 */
		if (bp->common.shmem_base == 0xFFFFFFFF) {
			bp->flags |= NO_MCP_FLAG;
			return -ENODEV;
		}

		if (bp->common.shmem_base) {
			val = SHMEM_RD(bp, validity_map[BP_PORT(bp)]);
			if (val & SHR_MEM_VALIDITY_MB)
				return 0;
		}

		bnx2x_mcp_wait_one(bp);

	} while (cnt++ < (MCP_TIMEOUT / MCP_ONE_TIMEOUT));

	BNX2X_ERR("BAD MCP validity signature\n");

	return -ENODEV;
}