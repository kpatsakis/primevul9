static void i40e_restore_vlan(struct i40e_vsi *vsi)
{
	u16 vid;

	if (!vsi->netdev)
		return;

	if (vsi->netdev->features & NETIF_F_HW_VLAN_CTAG_RX)
		i40e_vlan_stripping_enable(vsi);
	else
		i40e_vlan_stripping_disable(vsi);

	for_each_set_bit(vid, vsi->active_vlans, VLAN_N_VID)
		i40e_vlan_rx_add_vid_up(vsi->netdev, htons(ETH_P_8021Q),
					vid);
}