int nsh_hdr_from_nlattr(const struct nlattr *attr,
			struct nshhdr *nh, size_t size)
{
	struct nlattr *a;
	int rem;
	u8 flags = 0;
	u8 ttl = 0;
	int mdlen = 0;

	/* validate_nsh has check this, so we needn't do duplicate check here
	 */
	if (size < NSH_BASE_HDR_LEN)
		return -ENOBUFS;

	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);

		switch (type) {
		case OVS_NSH_KEY_ATTR_BASE: {
			const struct ovs_nsh_key_base *base = nla_data(a);

			flags = base->flags;
			ttl = base->ttl;
			nh->np = base->np;
			nh->mdtype = base->mdtype;
			nh->path_hdr = base->path_hdr;
			break;
		}
		case OVS_NSH_KEY_ATTR_MD1:
			mdlen = nla_len(a);
			if (mdlen > size - NSH_BASE_HDR_LEN)
				return -ENOBUFS;
			memcpy(&nh->md1, nla_data(a), mdlen);
			break;

		case OVS_NSH_KEY_ATTR_MD2:
			mdlen = nla_len(a);
			if (mdlen > size - NSH_BASE_HDR_LEN)
				return -ENOBUFS;
			memcpy(&nh->md2, nla_data(a), mdlen);
			break;

		default:
			return -EINVAL;
		}
	}

	/* nsh header length  = NSH_BASE_HDR_LEN + mdlen */
	nh->ver_flags_ttl_len = 0;
	nsh_set_flags_ttl_len(nh, flags, ttl, NSH_BASE_HDR_LEN + mdlen);

	return 0;
}