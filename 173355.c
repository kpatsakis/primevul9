static int bnx2x_reset_mcp_comp(struct bnx2x *bp, u32 magic_val)
{
	int rc = bnx2x_init_shmem(bp);

	/* Restore the `magic' bit value */
	if (!CHIP_IS_E1(bp))
		bnx2x_clp_reset_done(bp, magic_val);

	return rc;
}