static void i40e_free_macvlan_channels(struct i40e_vsi *vsi)
{
	struct i40e_channel *ch, *ch_tmp;
	int ret;

	if (list_empty(&vsi->macvlan_list))
		return;

	list_for_each_entry_safe(ch, ch_tmp, &vsi->macvlan_list, list) {
		struct i40e_vsi *parent_vsi;

		if (i40e_is_channel_macvlan(ch)) {
			i40e_reset_ch_rings(vsi, ch);
			clear_bit(ch->fwd->bit_no, vsi->fwd_bitmask);
			netdev_unbind_sb_channel(vsi->netdev, ch->fwd->netdev);
			netdev_set_sb_channel(ch->fwd->netdev, 0);
			kfree(ch->fwd);
			ch->fwd = NULL;
		}

		list_del(&ch->list);
		parent_vsi = ch->parent_vsi;
		if (!parent_vsi || !ch->initialized) {
			kfree(ch);
			continue;
		}

		/* remove the VSI */
		ret = i40e_aq_delete_element(&vsi->back->hw, ch->seid,
					     NULL);
		if (ret)
			dev_err(&vsi->back->pdev->dev,
				"unable to remove channel (%d) for parent VSI(%d)\n",
				ch->seid, parent_vsi->seid);
		kfree(ch);
	}
	vsi->macvlan_cnt = 0;
}