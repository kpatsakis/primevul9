static int validate_geneve_opts(struct sw_flow_key *key)
{
	struct geneve_opt *option;
	int opts_len = key->tun_opts_len;
	bool crit_opt = false;

	option = (struct geneve_opt *)TUN_METADATA_OPTS(key, key->tun_opts_len);
	while (opts_len > 0) {
		int len;

		if (opts_len < sizeof(*option))
			return -EINVAL;

		len = sizeof(*option) + option->length * 4;
		if (len > opts_len)
			return -EINVAL;

		crit_opt |= !!(option->type & GENEVE_CRIT_OPT_TYPE);

		option = (struct geneve_opt *)((u8 *)option + len);
		opts_len -= len;
	}

	key->tun_key.tun_flags |= crit_opt ? TUNNEL_CRIT_OPT : 0;

	return 0;
}