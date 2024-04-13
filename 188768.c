static i40e_status i40e_add_macvlan_filter(struct i40e_hw *hw, u16 seid,
					   const u8 *macaddr, int *aq_err)
{
	struct i40e_aqc_add_macvlan_element_data element;
	i40e_status status;
	u16 cmd_flags = 0;

	ether_addr_copy(element.mac_addr, macaddr);
	element.vlan_tag = 0;
	element.queue_number = 0;
	element.match_method = I40E_AQC_MM_ERR_NO_RES;
	cmd_flags |= I40E_AQC_MACVLAN_ADD_PERFECT_MATCH;
	element.flags = cpu_to_le16(cmd_flags);
	status = i40e_aq_add_macvlan(hw, seid, &element, 1, NULL);
	*aq_err = hw->aq.asq_last_status;

	return status;
}