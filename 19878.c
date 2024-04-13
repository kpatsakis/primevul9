static int genev_tun_opt_from_nlattr(const struct nlattr *a,
				     struct sw_flow_match *match, bool is_mask,
				     bool log)
{
	unsigned long opt_key_offset;

	if (nla_len(a) > sizeof(match->key->tun_opts)) {
		OVS_NLERR(log, "Geneve option length err (len %d, max %zu).",
			  nla_len(a), sizeof(match->key->tun_opts));
		return -EINVAL;
	}

	if (nla_len(a) % 4 != 0) {
		OVS_NLERR(log, "Geneve opt len %d is not a multiple of 4.",
			  nla_len(a));
		return -EINVAL;
	}

	/* We need to record the length of the options passed
	 * down, otherwise packets with the same format but
	 * additional options will be silently matched.
	 */
	if (!is_mask) {
		SW_FLOW_KEY_PUT(match, tun_opts_len, nla_len(a),
				false);
	} else {
		/* This is somewhat unusual because it looks at
		 * both the key and mask while parsing the
		 * attributes (and by extension assumes the key
		 * is parsed first). Normally, we would verify
		 * that each is the correct length and that the
		 * attributes line up in the validate function.
		 * However, that is difficult because this is
		 * variable length and we won't have the
		 * information later.
		 */
		if (match->key->tun_opts_len != nla_len(a)) {
			OVS_NLERR(log, "Geneve option len %d != mask len %d",
				  match->key->tun_opts_len, nla_len(a));
			return -EINVAL;
		}

		SW_FLOW_KEY_PUT(match, tun_opts_len, 0xff, true);
	}

	opt_key_offset = TUN_METADATA_OFFSET(nla_len(a));
	SW_FLOW_KEY_MEMCPY_OFFSET(match, opt_key_offset, nla_data(a),
				  nla_len(a), is_mask);
	return 0;
}