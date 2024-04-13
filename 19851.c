int nsh_key_from_nlattr(const struct nlattr *attr,
			struct ovs_key_nsh *nsh, struct ovs_key_nsh *nsh_mask)
{
	struct nlattr *a;
	int rem;

	/* validate_nsh has check this, so we needn't do duplicate check here
	 */
	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);

		switch (type) {
		case OVS_NSH_KEY_ATTR_BASE: {
			const struct ovs_nsh_key_base *base = nla_data(a);
			const struct ovs_nsh_key_base *base_mask = base + 1;

			nsh->base = *base;
			nsh_mask->base = *base_mask;
			break;
		}
		case OVS_NSH_KEY_ATTR_MD1: {
			const struct ovs_nsh_key_md1 *md1 = nla_data(a);
			const struct ovs_nsh_key_md1 *md1_mask = md1 + 1;

			memcpy(nsh->context, md1->context, sizeof(*md1));
			memcpy(nsh_mask->context, md1_mask->context,
			       sizeof(*md1_mask));
			break;
		}
		case OVS_NSH_KEY_ATTR_MD2:
			/* Not supported yet */
			return -ENOTSUPP;
		default:
			return -EINVAL;
		}
	}

	return 0;
}