static int mwifiex_update_curr_bss_params(struct mwifiex_private *priv,
					  struct cfg80211_bss *bss)
{
	struct mwifiex_bssdescriptor *bss_desc;
	int ret;

	/* Allocate and fill new bss descriptor */
	bss_desc = kzalloc(sizeof(struct mwifiex_bssdescriptor), GFP_KERNEL);
	if (!bss_desc)
		return -ENOMEM;

	ret = mwifiex_fill_new_bss_desc(priv, bss, bss_desc);
	if (ret)
		goto done;

	ret = mwifiex_check_network_compatibility(priv, bss_desc);
	if (ret)
		goto done;

	spin_lock_bh(&priv->curr_bcn_buf_lock);
	/* Make a copy of current BSSID descriptor */
	memcpy(&priv->curr_bss_params.bss_descriptor, bss_desc,
	       sizeof(priv->curr_bss_params.bss_descriptor));

	/* The contents of beacon_ie will be copied to its own buffer
	 * in mwifiex_save_curr_bcn()
	 */
	mwifiex_save_curr_bcn(priv);
	spin_unlock_bh(&priv->curr_bcn_buf_lock);

done:
	/* beacon_ie buffer was allocated in function
	 * mwifiex_fill_new_bss_desc(). Free it now.
	 */
	kfree(bss_desc->beacon_buf);
	kfree(bss_desc);
	return 0;
}