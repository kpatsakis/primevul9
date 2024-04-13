static void i40e_setup_pf_switch_element(struct i40e_pf *pf,
				struct i40e_aqc_switch_config_element_resp *ele,
				u16 num_reported, bool printconfig)
{
	u16 downlink_seid = le16_to_cpu(ele->downlink_seid);
	u16 uplink_seid = le16_to_cpu(ele->uplink_seid);
	u8 element_type = ele->element_type;
	u16 seid = le16_to_cpu(ele->seid);

	if (printconfig)
		dev_info(&pf->pdev->dev,
			 "type=%d seid=%d uplink=%d downlink=%d\n",
			 element_type, seid, uplink_seid, downlink_seid);

	switch (element_type) {
	case I40E_SWITCH_ELEMENT_TYPE_MAC:
		pf->mac_seid = seid;
		break;
	case I40E_SWITCH_ELEMENT_TYPE_VEB:
		/* Main VEB? */
		if (uplink_seid != pf->mac_seid)
			break;
		if (pf->lan_veb >= I40E_MAX_VEB) {
			int v;

			/* find existing or else empty VEB */
			for (v = 0; v < I40E_MAX_VEB; v++) {
				if (pf->veb[v] && (pf->veb[v]->seid == seid)) {
					pf->lan_veb = v;
					break;
				}
			}
			if (pf->lan_veb >= I40E_MAX_VEB) {
				v = i40e_veb_mem_alloc(pf);
				if (v < 0)
					break;
				pf->lan_veb = v;
			}
		}
		if (pf->lan_veb >= I40E_MAX_VEB)
			break;

		pf->veb[pf->lan_veb]->seid = seid;
		pf->veb[pf->lan_veb]->uplink_seid = pf->mac_seid;
		pf->veb[pf->lan_veb]->pf = pf;
		pf->veb[pf->lan_veb]->veb_idx = I40E_NO_VEB;
		break;
	case I40E_SWITCH_ELEMENT_TYPE_VSI:
		if (num_reported != 1)
			break;
		/* This is immediately after a reset so we can assume this is
		 * the PF's VSI
		 */
		pf->mac_seid = uplink_seid;
		pf->pf_seid = downlink_seid;
		pf->main_vsi_seid = seid;
		if (printconfig)
			dev_info(&pf->pdev->dev,
				 "pf_seid=%d main_vsi_seid=%d\n",
				 pf->pf_seid, pf->main_vsi_seid);
		break;
	case I40E_SWITCH_ELEMENT_TYPE_PF:
	case I40E_SWITCH_ELEMENT_TYPE_VF:
	case I40E_SWITCH_ELEMENT_TYPE_EMP:
	case I40E_SWITCH_ELEMENT_TYPE_BMC:
	case I40E_SWITCH_ELEMENT_TYPE_PE:
	case I40E_SWITCH_ELEMENT_TYPE_PA:
		/* ignore these for now */
		break;
	default:
		dev_info(&pf->pdev->dev, "unknown element type=%d seid=%d\n",
			 element_type, seid);
		break;
	}
}