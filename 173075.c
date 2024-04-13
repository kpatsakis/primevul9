void bnx2x_link_set(struct bnx2x *bp)
{
	if (!BP_NOMCP(bp)) {
		bnx2x_acquire_phy_lock(bp);
		bnx2x_phy_init(&bp->link_params, &bp->link_vars);
		bnx2x_release_phy_lock(bp);

		bnx2x_init_dropless_fc(bp);

		bnx2x_calc_fc_adv(bp);
	} else
		BNX2X_ERR("Bootcode is missing - can not set link\n");
}