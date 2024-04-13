int i40e_is_vsi_uplink_mode_veb(struct i40e_vsi *vsi)
{
	struct i40e_veb *veb;
	struct i40e_pf *pf = vsi->back;

	/* Uplink is not a bridge so default to VEB */
	if (vsi->veb_idx >= I40E_MAX_VEB)
		return 1;

	veb = pf->veb[vsi->veb_idx];
	if (!veb) {
		dev_info(&pf->pdev->dev,
			 "There is no veb associated with the bridge\n");
		return -ENOENT;
	}

	/* Uplink is a bridge in VEPA mode */
	if (veb->bridge_mode & BRIDGE_MODE_VEPA) {
		return 0;
	} else {
		/* Uplink is a bridge in VEB mode */
		return 1;
	}

	/* VEPA is now default bridge, so return 0 */
	return 0;
}