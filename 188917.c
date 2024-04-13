static int i40e_get_phys_port_id(struct net_device *netdev,
				 struct netdev_phys_item_id *ppid)
{
	struct i40e_netdev_priv *np = netdev_priv(netdev);
	struct i40e_pf *pf = np->vsi->back;
	struct i40e_hw *hw = &pf->hw;

	if (!(pf->hw_features & I40E_HW_PORT_ID_VALID))
		return -EOPNOTSUPP;

	ppid->id_len = min_t(int, sizeof(hw->mac.port_addr), sizeof(ppid->id));
	memcpy(ppid->id, hw->mac.port_addr, ppid->id_len);

	return 0;
}