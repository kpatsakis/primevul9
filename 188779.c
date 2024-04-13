static void i40e_fwd_del(struct net_device *netdev, void *vdev)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_fwd_adapter *fwd = vdev;
	struct i40e_channel *ch, *ch_tmp;
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	struct i40e_hw *hw = &pf->hw;
	int aq_err, ret = 0;

	/* Find the channel associated with the macvlan and del mac filter */
	list_for_each_entry_safe(ch, ch_tmp, &vsi->macvlan_list, list) {
		if (i40e_is_channel_macvlan(ch) &&
		    ether_addr_equal(i40e_channel_mac(ch),
				     fwd->netdev->dev_addr)) {
			ret = i40e_del_macvlan_filter(hw, ch->seid,
						      i40e_channel_mac(ch),
						      &aq_err);
			if (!ret) {
				/* Reset queue contexts */
				i40e_reset_ch_rings(vsi, ch);
				clear_bit(ch->fwd->bit_no, vsi->fwd_bitmask);
				netdev_unbind_sb_channel(netdev, fwd->netdev);
				netdev_set_sb_channel(fwd->netdev, 0);
				kfree(ch->fwd);
				ch->fwd = NULL;
			} else {
				dev_info(&pf->pdev->dev,
					 "Error deleting mac filter on macvlan err %s, aq_err %s\n",
					  i40e_stat_str(hw, ret),
					  i40e_aq_str(hw, aq_err));
			}
			break;
		}
	}
}