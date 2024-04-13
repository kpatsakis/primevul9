void bnx2x_force_link_reset(struct bnx2x *bp)
{
	bnx2x_acquire_phy_lock(bp);
	bnx2x_link_reset(&bp->link_params, &bp->link_vars, 1);
	bnx2x_release_phy_lock(bp);
}