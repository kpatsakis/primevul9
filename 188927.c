static int i40e_ndo_bridge_setlink(struct net_device *dev,
				   struct nlmsghdr *nlh,
				   u16 flags,
				   struct netlink_ext_ack *extack)
{
	struct i40e_netdev_priv *np = netdev_priv(dev);
	struct i40e_vsi *vsi = np->vsi;
	struct i40e_pf *pf = vsi->back;
	struct i40e_veb *veb = NULL;
	struct nlattr *attr, *br_spec;
	int i, rem;

	/* Only for PF VSI for now */
	if (vsi->seid != pf->vsi[pf->lan_vsi]->seid)
		return -EOPNOTSUPP;

	/* Find the HW bridge for PF VSI */
	for (i = 0; i < I40E_MAX_VEB && !veb; i++) {
		if (pf->veb[i] && pf->veb[i]->seid == vsi->uplink_seid)
			veb = pf->veb[i];
	}

	br_spec = nlmsg_find_attr(nlh, sizeof(struct ifinfomsg), IFLA_AF_SPEC);

	nla_for_each_nested(attr, br_spec, rem) {
		__u16 mode;

		if (nla_type(attr) != IFLA_BRIDGE_MODE)
			continue;

		mode = nla_get_u16(attr);
		if ((mode != BRIDGE_MODE_VEPA) &&
		    (mode != BRIDGE_MODE_VEB))
			return -EINVAL;

		/* Insert a new HW bridge */
		if (!veb) {
			veb = i40e_veb_setup(pf, 0, vsi->uplink_seid, vsi->seid,
					     vsi->tc_config.enabled_tc);
			if (veb) {
				veb->bridge_mode = mode;
				i40e_config_bridge_mode(veb);
			} else {
				/* No Bridge HW offload available */
				return -ENOENT;
			}
			break;
		} else if (mode != veb->bridge_mode) {
			/* Existing HW bridge but different mode needs reset */
			veb->bridge_mode = mode;
			/* TODO: If no VFs or VMDq VSIs, disallow VEB mode */
			if (mode == BRIDGE_MODE_VEB)
				pf->flags |= I40E_FLAG_VEB_MODE_ENABLED;
			else
				pf->flags &= ~I40E_FLAG_VEB_MODE_ENABLED;
			i40e_do_reset(pf, I40E_PF_RESET_FLAG, true);
			break;
		}
	}

	return 0;
}