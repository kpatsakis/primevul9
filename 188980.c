static int i40e_ndo_bridge_getlink(struct sk_buff *skb, u32 pid, u32 seq,
				   struct net_device *dev,
				   u32 __always_unused filter_mask,
				   int nlflags)
{
	struct i40e_netdev_priv *np = netdev_priv(dev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	struct i40e_veb *veb = NULL;
	int i;

	/* Only for PF VSI for now */
	if (vsi->seid != pf->vsi[pf->lan_vsi]->seid)
		return -EOPNOTSUPP;

	/* Find the HW bridge for the PF VSI */
	for (i = 0; i < I40E_MAX_VEB && !veb; i++) {
		if (pf->veb[i] && pf->veb[i]->seid == vsi->uplink_seid)
			veb = pf->veb[i];
	}

	if (!veb)
		return 0;

	return ndo_dflt_bridge_getlink(skb, pid, seq, dev, veb->bridge_mode,
				       0, 0, nlflags, filter_mask, NULL);
}