static int i40e_add_channel(struct i40e_pf *pf, u16 uplink_seid,
			    struct i40e_channel *ch)
{
	struct i40e_hw *hw = &pf->hw;
	struct i40e_vsi_context ctxt;
	u8 enabled_tc = 0x1; /* TC0 enabled */
	int ret;

	if (ch->type != I40E_VSI_VMDQ2) {
		dev_info(&pf->pdev->dev,
			 "add new vsi failed, ch->type %d\n", ch->type);
		return -EINVAL;
	}

	memset(&ctxt, 0, sizeof(ctxt));
	ctxt.pf_num = hw->pf_id;
	ctxt.vf_num = 0;
	ctxt.uplink_seid = uplink_seid;
	ctxt.connection_type = I40E_AQ_VSI_CONN_TYPE_NORMAL;
	if (ch->type == I40E_VSI_VMDQ2)
		ctxt.flags = I40E_AQ_VSI_TYPE_VMDQ2;

	if (pf->flags & I40E_FLAG_VEB_MODE_ENABLED) {
		ctxt.info.valid_sections |=
		     cpu_to_le16(I40E_AQ_VSI_PROP_SWITCH_VALID);
		ctxt.info.switch_id =
		   cpu_to_le16(I40E_AQ_VSI_SW_ID_FLAG_ALLOW_LB);
	}

	/* Set queue map for a given VSI context */
	i40e_channel_setup_queue_map(pf, &ctxt, ch);

	/* Now time to create VSI */
	ret = i40e_aq_add_vsi(hw, &ctxt, NULL);
	if (ret) {
		dev_info(&pf->pdev->dev,
			 "add new vsi failed, err %s aq_err %s\n",
			 i40e_stat_str(&pf->hw, ret),
			 i40e_aq_str(&pf->hw,
				     pf->hw.aq.asq_last_status));
		return -ENOENT;
	}

	/* Success, update channel, set enabled_tc only if the channel
	 * is not a macvlan
	 */
	ch->enabled_tc = !i40e_is_channel_macvlan(ch) && enabled_tc;
	ch->seid = ctxt.seid;
	ch->vsi_number = ctxt.vsi_number;
	ch->stat_counter_idx = cpu_to_le16(ctxt.info.stat_counter_idx);

	/* copy just the sections touched not the entire info
	 * since not all sections are valid as returned by
	 * update vsi params
	 */
	ch->info.mapping_flags = ctxt.info.mapping_flags;
	memcpy(&ch->info.queue_mapping,
	       &ctxt.info.queue_mapping, sizeof(ctxt.info.queue_mapping));
	memcpy(&ch->info.tc_mapping, ctxt.info.tc_mapping,
	       sizeof(ctxt.info.tc_mapping));

	return 0;
}