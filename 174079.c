static int mwifiex_save_hidden_ssid_channels(struct mwifiex_private *priv,
					     struct cfg80211_bss *bss)
{
	struct mwifiex_bssdescriptor *bss_desc;
	int ret;
	int chid;

	/* Allocate and fill new bss descriptor */
	bss_desc = kzalloc(sizeof(*bss_desc), GFP_KERNEL);
	if (!bss_desc)
		return -ENOMEM;

	ret = mwifiex_fill_new_bss_desc(priv, bss, bss_desc);
	if (ret)
		goto done;

	if (mwifiex_is_hidden_ssid(&bss_desc->ssid)) {
		mwifiex_dbg(priv->adapter, INFO, "found hidden SSID\n");
		for (chid = 0 ; chid < MWIFIEX_USER_SCAN_CHAN_MAX; chid++) {
			if (priv->hidden_chan[chid].chan_number ==
			    bss->channel->hw_value)
				break;

			if (!priv->hidden_chan[chid].chan_number) {
				priv->hidden_chan[chid].chan_number =
					bss->channel->hw_value;
				priv->hidden_chan[chid].radio_type =
					bss->channel->band;
				priv->hidden_chan[chid].scan_type =
					MWIFIEX_SCAN_TYPE_ACTIVE;
				break;
			}
		}
	}

done:
	/* beacon_ie buffer was allocated in function
	 * mwifiex_fill_new_bss_desc(). Free it now.
	 */
	kfree(bss_desc->beacon_buf);
	kfree(bss_desc);
	return 0;
}