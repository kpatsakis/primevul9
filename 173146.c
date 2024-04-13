static int bnx2x_get_leader_lock_resource(struct bnx2x *bp)
{
	if (BP_PATH(bp))
		return HW_LOCK_RESOURCE_RECOVERY_LEADER_1;
	else
		return HW_LOCK_RESOURCE_RECOVERY_LEADER_0;
}