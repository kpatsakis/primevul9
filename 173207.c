static int __bnx2x_vlan_configure_vid(struct bnx2x *bp, u16 vid, bool add)
{
	int rc;

	if (IS_PF(bp)) {
		unsigned long ramrod_flags = 0;

		__set_bit(RAMROD_COMP_WAIT, &ramrod_flags);
		rc = bnx2x_set_vlan_one(bp, vid, &bp->sp_objs->vlan_obj,
					add, &ramrod_flags);
	} else {
		rc = bnx2x_vfpf_update_vlan(bp, vid, bp->fp->index, add);
	}

	return rc;
}