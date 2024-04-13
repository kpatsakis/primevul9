static void i40e_switch_branch_release(struct i40e_veb *branch)
{
	struct i40e_pf *pf = branch->pf;
	u16 branch_seid = branch->seid;
	u16 veb_idx = branch->idx;
	int i;

	/* release any VEBs on this VEB - RECURSION */
	for (i = 0; i < I40E_MAX_VEB; i++) {
		if (!pf->veb[i])
			continue;
		if (pf->veb[i]->uplink_seid == branch->seid)
			i40e_switch_branch_release(pf->veb[i]);
	}

	/* Release the VSIs on this VEB, but not the owner VSI.
	 *
	 * NOTE: Removing the last VSI on a VEB has the SIDE EFFECT of removing
	 *       the VEB itself, so don't use (*branch) after this loop.
	 */
	for (i = 0; i < pf->num_alloc_vsi; i++) {
		if (!pf->vsi[i])
			continue;
		if (pf->vsi[i]->uplink_seid == branch_seid &&
		   (pf->vsi[i]->flags & I40E_VSI_FLAG_VEB_OWNER) == 0) {
			i40e_vsi_release(pf->vsi[i]);
		}
	}

	/* There's one corner case where the VEB might not have been
	 * removed, so double check it here and remove it if needed.
	 * This case happens if the veb was created from the debugfs
	 * commands and no VSIs were added to it.
	 */
	if (pf->veb[veb_idx])
		i40e_veb_release(pf->veb[veb_idx]);
}