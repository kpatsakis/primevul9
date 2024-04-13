i40e_set_cld_element(struct i40e_cloud_filter *filter,
		     struct i40e_aqc_cloud_filters_element_data *cld)
{
	int i, j;
	u32 ipa;

	memset(cld, 0, sizeof(*cld));
	ether_addr_copy(cld->outer_mac, filter->dst_mac);
	ether_addr_copy(cld->inner_mac, filter->src_mac);

	if (filter->n_proto != ETH_P_IP && filter->n_proto != ETH_P_IPV6)
		return;

	if (filter->n_proto == ETH_P_IPV6) {
#define IPV6_MAX_INDEX	(ARRAY_SIZE(filter->dst_ipv6) - 1)
		for (i = 0, j = 0; i < ARRAY_SIZE(filter->dst_ipv6);
		     i++, j += 2) {
			ipa = be32_to_cpu(filter->dst_ipv6[IPV6_MAX_INDEX - i]);
			ipa = cpu_to_le32(ipa);
			memcpy(&cld->ipaddr.raw_v6.data[j], &ipa, sizeof(ipa));
		}
	} else {
		ipa = be32_to_cpu(filter->dst_ipv4);
		memcpy(&cld->ipaddr.v4.data, &ipa, sizeof(ipa));
	}

	cld->inner_vlan = cpu_to_le16(ntohs(filter->vlan_id));

	/* tenant_id is not supported by FW now, once the support is enabled
	 * fill the cld->tenant_id with cpu_to_le32(filter->tenant_id)
	 */
	if (filter->tenant_id)
		return;
}