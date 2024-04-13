static i40e_status i40e_del_macvlan_filter(struct i40e_hw *hw, u16 seid,
					   const u8 *macaddr, int *aq_err)
{
	struct i40e_aqc_remove_macvlan_element_data element;
	i40e_status status;

	memset(&element, 0, sizeof(element));
	ether_addr_copy(element.mac_addr, macaddr);
	element.vlan_tag = 0;
	element.flags = I40E_AQC_MACVLAN_DEL_PERFECT_MATCH;
	status = i40e_aq_remove_macvlan(hw, seid, &element, 1, NULL);
	*aq_err = hw->aq.asq_last_status;

	return status;
}