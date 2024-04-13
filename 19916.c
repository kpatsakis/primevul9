static int validate_vlan_mask_from_nlattrs(const struct sw_flow_match *match,
					   u64 key_attrs, bool inner,
					   const struct nlattr **a, bool log)
{
	__be16 tci = 0;
	__be16 tpid = 0;
	bool encap_valid = !!(match->key->eth.vlan.tci &
			      htons(VLAN_CFI_MASK));
	bool i_encap_valid = !!(match->key->eth.cvlan.tci &
				htons(VLAN_CFI_MASK));

	if (!(key_attrs & (1 << OVS_KEY_ATTR_ENCAP))) {
		/* Not a VLAN. */
		return 0;
	}

	if ((!inner && !encap_valid) || (inner && !i_encap_valid)) {
		OVS_NLERR(log, "Encap mask attribute is set for non-%s frame.",
			  (inner) ? "C-VLAN" : "VLAN");
		return -EINVAL;
	}

	if (a[OVS_KEY_ATTR_VLAN])
		tci = nla_get_be16(a[OVS_KEY_ATTR_VLAN]);

	if (a[OVS_KEY_ATTR_ETHERTYPE])
		tpid = nla_get_be16(a[OVS_KEY_ATTR_ETHERTYPE]);

	if (tpid != htons(0xffff)) {
		OVS_NLERR(log, "Must have an exact match on %s TPID (mask=%x).",
			  (inner) ? "C-VLAN" : "VLAN", ntohs(tpid));
		return -EINVAL;
	}
	if (!(tci & htons(VLAN_CFI_MASK))) {
		OVS_NLERR(log, "%s TCI mask does not have exact match for VLAN_CFI_MASK bit.",
			  (inner) ? "C-VLAN" : "VLAN");
		return -EINVAL;
	}

	return 1;
}