int bnx2x_vlan_reconfigure_vid(struct bnx2x *bp)
{
	struct bnx2x_vlan_entry *vlan;

	/* The hw forgot all entries after reload */
	list_for_each_entry(vlan, &bp->vlan_reg, link)
		vlan->hw = false;
	bp->vlan_cnt = 0;

	/* Don't set rx mode here. Our caller will do it. */
	bnx2x_vlan_configure(bp, false);

	return 0;
}