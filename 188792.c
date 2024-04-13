static void i40e_del_all_macvlans(struct i40e_vsi *vsi)
{
	struct i40e_channel *ch, *ch_tmp;
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	int aq_err, ret = 0;

	if (list_empty(&vsi->macvlan_list))
		return;

	list_for_each_entry_safe(ch, ch_tmp, &vsi->macvlan_list, list) {
		if (i40e_is_channel_macvlan(ch)) {
			ret = i40e_del_macvlan_filter(hw, ch->seid,
						      i40e_channel_mac(ch),
						      &aq_err);
			if (!ret) {
				/* Reset queue contexts */
				i40e_reset_ch_rings(vsi, ch);
				clear_bit(ch->fwd->bit_no, vsi->fwd_bitmask);
				netdev_unbind_sb_channel(vsi->netdev,
							 ch->fwd->netdev);
				netdev_set_sb_channel(ch->fwd->netdev, 0);
				kfree(ch->fwd);
				ch->fwd = NULL;
			}
		}
	}
}