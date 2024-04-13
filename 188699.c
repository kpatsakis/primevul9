int i40e_add_del_cloud_filter_big_buf(struct i40e_vsi *vsi,
				      struct i40e_cloud_filter *filter,
				      bool add)
{
	struct i40e_aqc_cloud_filters_element_bb cld_filter;
	struct i40e_pf *pf = vsi->back;
	int ret;

	/* Both (src/dst) valid mac_addr are not supported */
	if ((is_valid_ether_addr(filter->dst_mac) &&
	     is_valid_ether_addr(filter->src_mac)) ||
	    (is_multicast_ether_addr(filter->dst_mac) &&
	     is_multicast_ether_addr(filter->src_mac)))
		return -EOPNOTSUPP;

	/* Big buffer cloud filter needs 'L4 port' to be non-zero. Also, UDP
	 * ports are not supported via big buffer now.
	 */
	if (!filter->dst_port || filter->ip_proto == IPPROTO_UDP)
		return -EOPNOTSUPP;

	/* adding filter using src_port/src_ip is not supported at this stage */
	if (filter->src_port || filter->src_ipv4 ||
	    !ipv6_addr_any(&filter->ip.v6.src_ip6))
		return -EOPNOTSUPP;

	/* copy element needed to add cloud filter from filter */
	i40e_set_cld_element(filter, &cld_filter.element);

	if (is_valid_ether_addr(filter->dst_mac) ||
	    is_valid_ether_addr(filter->src_mac) ||
	    is_multicast_ether_addr(filter->dst_mac) ||
	    is_multicast_ether_addr(filter->src_mac)) {
		/* MAC + IP : unsupported mode */
		if (filter->dst_ipv4)
			return -EOPNOTSUPP;

		/* since we validated that L4 port must be valid before
		 * we get here, start with respective "flags" value
		 * and update if vlan is present or not
		 */
		cld_filter.element.flags =
			cpu_to_le16(I40E_AQC_ADD_CLOUD_FILTER_MAC_PORT);

		if (filter->vlan_id) {
			cld_filter.element.flags =
			cpu_to_le16(I40E_AQC_ADD_CLOUD_FILTER_MAC_VLAN_PORT);
		}

	} else if (filter->dst_ipv4 ||
		   !ipv6_addr_any(&filter->ip.v6.dst_ip6)) {
		cld_filter.element.flags =
				cpu_to_le16(I40E_AQC_ADD_CLOUD_FILTER_IP_PORT);
		if (filter->n_proto == ETH_P_IPV6)
			cld_filter.element.flags |=
				cpu_to_le16(I40E_AQC_ADD_CLOUD_FLAGS_IPV6);
		else
			cld_filter.element.flags |=
				cpu_to_le16(I40E_AQC_ADD_CLOUD_FLAGS_IPV4);
	} else {
		dev_err(&pf->pdev->dev,
			"either mac or ip has to be valid for cloud filter\n");
		return -EINVAL;
	}

	/* Now copy L4 port in Byte 6..7 in general fields */
	cld_filter.general_fields[I40E_AQC_ADD_CLOUD_FV_FLU_0X16_WORD0] =
						be16_to_cpu(filter->dst_port);

	if (add) {
		/* Validate current device switch mode, change if necessary */
		ret = i40e_validate_and_set_switch_mode(vsi);
		if (ret) {
			dev_err(&pf->pdev->dev,
				"failed to set switch mode, ret %d\n",
				ret);
			return ret;
		}

		ret = i40e_aq_add_cloud_filters_bb(&pf->hw, filter->seid,
						   &cld_filter, 1);
	} else {
		ret = i40e_aq_rem_cloud_filters_bb(&pf->hw, filter->seid,
						   &cld_filter, 1);
	}

	if (ret)
		dev_dbg(&pf->pdev->dev,
			"Failed to %s cloud filter(big buffer) err %d aq_err %d\n",
			add ? "add" : "delete", ret, pf->hw.aq.asq_last_status);
	else
		dev_info(&pf->pdev->dev,
			 "%s cloud filter for VSI: %d, L4 port: %d\n",
			 add ? "add" : "delete", filter->seid,
			 ntohs(filter->dst_port));
	return ret;
}