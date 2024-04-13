u8 bnx2x_link_test(struct bnx2x *bp, u8 is_serdes)
{
	u8 rc = 0;

	if (!BP_NOMCP(bp)) {
		bnx2x_acquire_phy_lock(bp);
		rc = bnx2x_test_link(&bp->link_params, &bp->link_vars,
				     is_serdes);
		bnx2x_release_phy_lock(bp);
	} else
		BNX2X_ERR("Bootcode is missing - can not test link\n");

	return rc;
}