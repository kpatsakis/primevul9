struct bond_vlan_tag *bond_verify_device_path(struct net_device *start_dev,
					      struct net_device *end_dev,
					      int level)
{
	struct bond_vlan_tag *tags;
	struct net_device *upper;
	struct list_head  *iter;

	if (start_dev == end_dev) {
		tags = kcalloc(level + 1, sizeof(*tags), GFP_ATOMIC);
		if (!tags)
			return ERR_PTR(-ENOMEM);
		tags[level].vlan_proto = VLAN_N_VID;
		return tags;
	}

	netdev_for_each_upper_dev_rcu(start_dev, upper, iter) {
		tags = bond_verify_device_path(upper, end_dev, level + 1);
		if (IS_ERR_OR_NULL(tags)) {
			if (IS_ERR(tags))
				return tags;
			continue;
		}
		if (is_vlan_dev(upper)) {
			tags[level].vlan_proto = vlan_dev_vlan_proto(upper);
			tags[level].vlan_id = vlan_dev_vlan_id(upper);
		}

		return tags;
	}

	return NULL;
}