static int vxlan_tun_opt_from_nlattr(const struct nlattr *attr,
				     struct sw_flow_match *match, bool is_mask,
				     bool log)
{
	struct nlattr *a;
	int rem;
	unsigned long opt_key_offset;
	struct vxlan_metadata opts;

	BUILD_BUG_ON(sizeof(opts) > sizeof(match->key->tun_opts));

	memset(&opts, 0, sizeof(opts));
	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);

		if (type > OVS_VXLAN_EXT_MAX) {
			OVS_NLERR(log, "VXLAN extension %d out of range max %d",
				  type, OVS_VXLAN_EXT_MAX);
			return -EINVAL;
		}

		if (!check_attr_len(nla_len(a),
				    ovs_vxlan_ext_key_lens[type].len)) {
			OVS_NLERR(log, "VXLAN extension %d has unexpected len %d expected %d",
				  type, nla_len(a),
				  ovs_vxlan_ext_key_lens[type].len);
			return -EINVAL;
		}

		switch (type) {
		case OVS_VXLAN_EXT_GBP:
			opts.gbp = nla_get_u32(a);
			break;
		default:
			OVS_NLERR(log, "Unknown VXLAN extension attribute %d",
				  type);
			return -EINVAL;
		}
	}
	if (rem) {
		OVS_NLERR(log, "VXLAN extension message has %d unknown bytes.",
			  rem);
		return -EINVAL;
	}

	if (!is_mask)
		SW_FLOW_KEY_PUT(match, tun_opts_len, sizeof(opts), false);
	else
		SW_FLOW_KEY_PUT(match, tun_opts_len, 0xff, true);

	opt_key_offset = TUN_METADATA_OFFSET(sizeof(opts));
	SW_FLOW_KEY_MEMCPY_OFFSET(match, opt_key_offset, &opts, sizeof(opts),
				  is_mask);
	return 0;
}