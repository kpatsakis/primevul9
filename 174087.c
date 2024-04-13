static bool mwifiex_is_hidden_ssid(struct cfg80211_ssid *ssid)
{
	int idx;

	for (idx = 0; idx < ssid->ssid_len; idx++) {
		if (ssid->ssid[idx])
			return false;
	}

	return true;
}