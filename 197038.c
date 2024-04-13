static int bond_do_ioctl(struct net_device *bond_dev, struct ifreq *ifr, int cmd)
{
	struct bonding *bond = netdev_priv(bond_dev);
	struct net_device *slave_dev = NULL;
	struct ifbond k_binfo;
	struct ifbond __user *u_binfo = NULL;
	struct ifslave k_sinfo;
	struct ifslave __user *u_sinfo = NULL;
	struct mii_ioctl_data *mii = NULL;
	struct bond_opt_value newval;
	struct net *net;
	int res = 0;

	netdev_dbg(bond_dev, "bond_ioctl: cmd=%d\n", cmd);

	switch (cmd) {
	case SIOCGMIIPHY:
		mii = if_mii(ifr);
		if (!mii)
			return -EINVAL;

		mii->phy_id = 0;
		fallthrough;
	case SIOCGMIIREG:
		/* We do this again just in case we were called by SIOCGMIIREG
		 * instead of SIOCGMIIPHY.
		 */
		mii = if_mii(ifr);
		if (!mii)
			return -EINVAL;

		if (mii->reg_num == 1) {
			mii->val_out = 0;
			if (netif_carrier_ok(bond->dev))
				mii->val_out = BMSR_LSTATUS;
		}

		return 0;
	case BOND_INFO_QUERY_OLD:
	case SIOCBONDINFOQUERY:
		u_binfo = (struct ifbond __user *)ifr->ifr_data;

		if (copy_from_user(&k_binfo, u_binfo, sizeof(ifbond)))
			return -EFAULT;

		bond_info_query(bond_dev, &k_binfo);
		if (copy_to_user(u_binfo, &k_binfo, sizeof(ifbond)))
			return -EFAULT;

		return 0;
	case BOND_SLAVE_INFO_QUERY_OLD:
	case SIOCBONDSLAVEINFOQUERY:
		u_sinfo = (struct ifslave __user *)ifr->ifr_data;

		if (copy_from_user(&k_sinfo, u_sinfo, sizeof(ifslave)))
			return -EFAULT;

		res = bond_slave_info_query(bond_dev, &k_sinfo);
		if (res == 0 &&
		    copy_to_user(u_sinfo, &k_sinfo, sizeof(ifslave)))
			return -EFAULT;

		return res;
	default:
		break;
	}

	net = dev_net(bond_dev);

	if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
		return -EPERM;

	slave_dev = __dev_get_by_name(net, ifr->ifr_slave);

	slave_dbg(bond_dev, slave_dev, "slave_dev=%p:\n", slave_dev);

	if (!slave_dev)
		return -ENODEV;

	switch (cmd) {
	case BOND_ENSLAVE_OLD:
	case SIOCBONDENSLAVE:
		res = bond_enslave(bond_dev, slave_dev, NULL);
		break;
	case BOND_RELEASE_OLD:
	case SIOCBONDRELEASE:
		res = bond_release(bond_dev, slave_dev);
		break;
	case BOND_SETHWADDR_OLD:
	case SIOCBONDSETHWADDR:
		res = bond_set_dev_addr(bond_dev, slave_dev);
		break;
	case BOND_CHANGE_ACTIVE_OLD:
	case SIOCBONDCHANGEACTIVE:
		bond_opt_initstr(&newval, slave_dev->name);
		res = __bond_opt_set_notify(bond, BOND_OPT_ACTIVE_SLAVE,
					    &newval);
		break;
	default:
		res = -EOPNOTSUPP;
	}

	return res;
}