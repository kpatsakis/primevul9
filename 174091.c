mwifiex_ssid_cmp(struct cfg80211_ssid *ssid1, struct cfg80211_ssid *ssid2)
{
	if (!ssid1 || !ssid2 || (ssid1->ssid_len != ssid2->ssid_len))
		return -1;
	return memcmp(ssid1->ssid, ssid2->ssid, ssid1->ssid_len);
}