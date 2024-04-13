static void i40e_rm_default_mac_filter(struct i40e_vsi *vsi, u8 *macaddr)
{
	struct i40e_aqc_remove_macvlan_element_data element;
	struct i40e_pf *pf = vsi->back;

	/* Only appropriate for the PF main VSI */
	if (vsi->type != I40E_VSI_MAIN)
		return;

	memset(&element, 0, sizeof(element));
	ether_addr_copy(element.mac_addr, macaddr);
	element.vlan_tag = 0;
	/* Ignore error returns, some firmware does it this way... */
	element.flags = I40E_AQC_MACVLAN_DEL_PERFECT_MATCH;
	i40e_aq_remove_macvlan(&pf->hw, vsi->seid, &element, 1, NULL);

	memset(&element, 0, sizeof(element));
	ether_addr_copy(element.mac_addr, macaddr);
	element.vlan_tag = 0;
	/* ...and some firmware does it this way. */
	element.flags = I40E_AQC_MACVLAN_DEL_PERFECT_MATCH |
			I40E_AQC_MACVLAN_DEL_IGNORE_VLAN;
	i40e_aq_remove_macvlan(&pf->hw, vsi->seid, &element, 1, NULL);
}