static int __parse_flow_nlattrs(const struct nlattr *attr,
				const struct nlattr *a[],
				u64 *attrsp, bool log, bool nz)
{
	const struct nlattr *nla;
	u64 attrs;
	int rem;

	attrs = *attrsp;
	nla_for_each_nested(nla, attr, rem) {
		u16 type = nla_type(nla);
		int expected_len;

		if (type > OVS_KEY_ATTR_MAX) {
			OVS_NLERR(log, "Key type %d is out of range max %d",
				  type, OVS_KEY_ATTR_MAX);
			return -EINVAL;
		}

		if (type == OVS_KEY_ATTR_PACKET_TYPE ||
		    type == OVS_KEY_ATTR_ND_EXTENSIONS ||
		    type == OVS_KEY_ATTR_TUNNEL_INFO) {
			OVS_NLERR(log, "Key type %d is not supported", type);
			return -EINVAL;
		}

		if (attrs & (1ULL << type)) {
			OVS_NLERR(log, "Duplicate key (type %d).", type);
			return -EINVAL;
		}

		expected_len = ovs_key_lens[type].len;
		if (!check_attr_len(nla_len(nla), expected_len)) {
			OVS_NLERR(log, "Key %d has unexpected len %d expected %d",
				  type, nla_len(nla), expected_len);
			return -EINVAL;
		}

		if (!nz || !is_all_zero(nla_data(nla), nla_len(nla))) {
			attrs |= 1ULL << type;
			a[type] = nla;
		}
	}
	if (rem) {
		OVS_NLERR(log, "Message has %d unknown bytes.", rem);
		return -EINVAL;
	}

	*attrsp = attrs;
	return 0;
}