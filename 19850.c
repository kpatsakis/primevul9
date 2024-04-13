static void nlattr_set(struct nlattr *attr, u8 val,
		       const struct ovs_len_tbl *tbl)
{
	struct nlattr *nla;
	int rem;

	/* The nlattr stream should already have been validated */
	nla_for_each_nested(nla, attr, rem) {
		if (tbl[nla_type(nla)].len == OVS_ATTR_NESTED)
			nlattr_set(nla, val, tbl[nla_type(nla)].next ? : tbl);
		else
			memset(nla_data(nla), val, nla_len(nla));

		if (nla_type(nla) == OVS_KEY_ATTR_CT_STATE)
			*(u32 *)nla_data(nla) &= CT_SUPPORTED_MASK;
	}
}