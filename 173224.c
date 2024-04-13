static bool bnx2x_trylock_leader_lock(struct bnx2x *bp)
{
	return bnx2x_trylock_hw_lock(bp, bnx2x_get_leader_lock_resource(bp));
}