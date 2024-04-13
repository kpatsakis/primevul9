int bnx2x_release_leader_lock(struct bnx2x *bp)
{
	return bnx2x_release_hw_lock(bp, bnx2x_get_leader_lock_resource(bp));
}