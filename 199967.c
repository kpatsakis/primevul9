static inline int is_beacon(__le16 fc)
{
	return (WLAN_FC_GET_STYPE(le16_to_cpu(fc)) == IEEE80211_STYPE_BEACON);
}