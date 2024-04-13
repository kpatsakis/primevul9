int mwifiex_request_scan(struct mwifiex_private *priv,
			 struct cfg80211_ssid *req_ssid)
{
	int ret;

	if (mutex_lock_interruptible(&priv->async_mutex)) {
		mwifiex_dbg(priv->adapter, ERROR,
			    "%s: acquire semaphore fail\n",
			    __func__);
		return -1;
	}

	priv->adapter->scan_wait_q_woken = false;

	if (req_ssid && req_ssid->ssid_len != 0)
		/* Specific SSID scan */
		ret = mwifiex_scan_specific_ssid(priv, req_ssid);
	else
		/* Normal scan */
		ret = mwifiex_scan_networks(priv, NULL);

	mutex_unlock(&priv->async_mutex);

	return ret;
}