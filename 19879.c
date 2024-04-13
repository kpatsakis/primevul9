static int validate_vlan_from_nlattrs(const struct sw_flow_match *match,
				      u64 key_attrs, bool inner,
				      const struct nlattr **a, bool log)
{
	__be16 tci = 0;

	if (!((key_attrs & (1 << OVS_KEY_ATTR_ETHERNET)) &&
	      (key_attrs & (1 << OVS_KEY_ATTR_ETHERTYPE)) &&
	       eth_type_vlan(nla_get_be16(a[OVS_KEY_ATTR_ETHERTYPE])))) {
		/* Not a VLAN. */
		return 0;
	}

	if (!((key_attrs & (1 << OVS_KEY_ATTR_VLAN)) &&
	      (key_attrs & (1 << OVS_KEY_ATTR_ENCAP)))) {
		OVS_NLERR(log, "Invalid %s frame", (inner) ? "C-VLAN" : "VLAN");
		return -EINVAL;
	}

	if (a[OVS_KEY_ATTR_VLAN])
		tci = nla_get_be16(a[OVS_KEY_ATTR_VLAN]);

	if (!(tci & htons(VLAN_CFI_MASK))) {
		if (tci) {
			OVS_NLERR(log, "%s TCI does not have VLAN_CFI_MASK bit set.",
				  (inner) ? "C-VLAN" : "VLAN");
			return -EINVAL;
		} else if (nla_len(a[OVS_KEY_ATTR_ENCAP])) {
			/* Corner case for truncated VLAN header. */
			OVS_NLERR(log, "Truncated %s header has non-zero encap attribute.",
				  (inner) ? "C-VLAN" : "VLAN");
			return -EINVAL;
		}
	}

	return 1;
}