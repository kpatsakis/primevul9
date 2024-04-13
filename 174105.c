mwifiex_save_curr_bcn(struct mwifiex_private *priv)
{
	struct mwifiex_bssdescriptor *curr_bss =
		&priv->curr_bss_params.bss_descriptor;

	if (!curr_bss->beacon_buf_size)
		return;

	/* allocate beacon buffer at 1st time; or if it's size has changed */
	if (!priv->curr_bcn_buf ||
	    priv->curr_bcn_size != curr_bss->beacon_buf_size) {
		priv->curr_bcn_size = curr_bss->beacon_buf_size;

		kfree(priv->curr_bcn_buf);
		priv->curr_bcn_buf = kmalloc(curr_bss->beacon_buf_size,
					     GFP_ATOMIC);
		if (!priv->curr_bcn_buf)
			return;
	}

	memcpy(priv->curr_bcn_buf, curr_bss->beacon_buf,
	       curr_bss->beacon_buf_size);
	mwifiex_dbg(priv->adapter, INFO,
		    "info: current beacon saved %d\n",
		    priv->curr_bcn_size);

	curr_bss->beacon_buf = priv->curr_bcn_buf;

	/* adjust the pointers in the current BSS descriptor */
	if (curr_bss->bcn_wpa_ie)
		curr_bss->bcn_wpa_ie =
			(struct ieee_types_vendor_specific *)
			(curr_bss->beacon_buf +
			 curr_bss->wpa_offset);

	if (curr_bss->bcn_rsn_ie)
		curr_bss->bcn_rsn_ie = (struct ieee_types_generic *)
			(curr_bss->beacon_buf +
			 curr_bss->rsn_offset);

	if (curr_bss->bcn_ht_cap)
		curr_bss->bcn_ht_cap = (struct ieee80211_ht_cap *)
			(curr_bss->beacon_buf +
			 curr_bss->ht_cap_offset);

	if (curr_bss->bcn_ht_oper)
		curr_bss->bcn_ht_oper = (struct ieee80211_ht_operation *)
			(curr_bss->beacon_buf +
			 curr_bss->ht_info_offset);

	if (curr_bss->bcn_vht_cap)
		curr_bss->bcn_vht_cap = (void *)(curr_bss->beacon_buf +
						 curr_bss->vht_cap_offset);

	if (curr_bss->bcn_vht_oper)
		curr_bss->bcn_vht_oper = (void *)(curr_bss->beacon_buf +
						  curr_bss->vht_info_offset);

	if (curr_bss->bcn_bss_co_2040)
		curr_bss->bcn_bss_co_2040 =
			(curr_bss->beacon_buf + curr_bss->bss_co_2040_offset);

	if (curr_bss->bcn_ext_cap)
		curr_bss->bcn_ext_cap = curr_bss->beacon_buf +
			curr_bss->ext_cap_offset;

	if (curr_bss->oper_mode)
		curr_bss->oper_mode = (void *)(curr_bss->beacon_buf +
					       curr_bss->oper_mode_offset);
}