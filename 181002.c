static void ath6kl_wmi_regdomain_event(struct wmi *wmi, u8 *datap, int len)
{
	struct ath6kl_wmi_regdomain *ev;
	struct country_code_to_enum_rd *country = NULL;
	struct reg_dmn_pair_mapping *regpair = NULL;
	char alpha2[2];
	u32 reg_code;

	ev = (struct ath6kl_wmi_regdomain *) datap;
	reg_code = le32_to_cpu(ev->reg_code);

	if ((reg_code >> ATH6KL_COUNTRY_RD_SHIFT) & COUNTRY_ERD_FLAG) {
		country = ath6kl_regd_find_country((u16) reg_code);
	} else if (!(((u16) reg_code & WORLD_SKU_MASK) == WORLD_SKU_PREFIX)) {
		regpair = ath6kl_get_regpair((u16) reg_code);
		country = ath6kl_regd_find_country_by_rd((u16) reg_code);
		if (regpair)
			ath6kl_dbg(ATH6KL_DBG_WMI, "Regpair used: 0x%0x\n",
				   regpair->reg_domain);
		else
			ath6kl_warn("Regpair not found reg_code 0x%0x\n",
				    reg_code);
	}

	if (country && wmi->parent_dev->wiphy_registered) {
		alpha2[0] = country->isoName[0];
		alpha2[1] = country->isoName[1];

		regulatory_hint(wmi->parent_dev->wiphy, alpha2);

		ath6kl_dbg(ATH6KL_DBG_WMI, "Country alpha2 being used: %c%c\n",
			   alpha2[0], alpha2[1]);
	}
}