static int parse_vlan_from_nlattrs(struct sw_flow_match *match,
				   u64 *key_attrs, const struct nlattr **a,
				   bool is_mask, bool log)
{
	int err;
	bool encap_valid = false;

	err = __parse_vlan_from_nlattrs(match, key_attrs, false, a,
					is_mask, log);
	if (err)
		return err;

	encap_valid = !!(match->key->eth.vlan.tci & htons(VLAN_CFI_MASK));
	if (encap_valid) {
		err = __parse_vlan_from_nlattrs(match, key_attrs, true, a,
						is_mask, log);
		if (err)
			return err;
	}

	return 0;
}