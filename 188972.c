static bool i40e_setup_channel(struct i40e_pf *pf, struct i40e_vsi *vsi,
			       struct i40e_channel *ch)
{
	u8 vsi_type;
	u16 seid;
	int ret;

	if (vsi->type == I40E_VSI_MAIN) {
		vsi_type = I40E_VSI_VMDQ2;
	} else {
		dev_err(&pf->pdev->dev, "unsupported parent vsi type(%d)\n",
			vsi->type);
		return false;
	}

	/* underlying switching element */
	seid = pf->vsi[pf->lan_vsi]->uplink_seid;

	/* create channel (VSI), configure TX rings */
	ret = i40e_setup_hw_channel(pf, vsi, ch, seid, vsi_type);
	if (ret) {
		dev_err(&pf->pdev->dev, "failed to setup hw_channel\n");
		return false;
	}

	return ch->initialized ? true : false;
}