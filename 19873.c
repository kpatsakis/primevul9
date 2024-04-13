static int erspan_tun_opt_from_nlattr(const struct nlattr *a,
				      struct sw_flow_match *match, bool is_mask,
				      bool log)
{
	unsigned long opt_key_offset;

	BUILD_BUG_ON(sizeof(struct erspan_metadata) >
		     sizeof(match->key->tun_opts));

	if (nla_len(a) > sizeof(match->key->tun_opts)) {
		OVS_NLERR(log, "ERSPAN option length err (len %d, max %zu).",
			  nla_len(a), sizeof(match->key->tun_opts));
		return -EINVAL;
	}

	if (!is_mask)
		SW_FLOW_KEY_PUT(match, tun_opts_len,
				sizeof(struct erspan_metadata), false);
	else
		SW_FLOW_KEY_PUT(match, tun_opts_len, 0xff, true);

	opt_key_offset = TUN_METADATA_OFFSET(nla_len(a));
	SW_FLOW_KEY_MEMCPY_OFFSET(match, opt_key_offset, nla_data(a),
				  nla_len(a), is_mask);
	return 0;
}