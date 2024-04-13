static int ieee80211_verify_qos_info(struct ieee80211_qos_information_element
				     *info_element, int sub_type)
{

	if (info_element->qui_subtype != sub_type)
		return -1;
	if (memcmp(info_element->qui, qos_oui, QOS_OUI_LEN))
		return -1;
	if (info_element->qui_type != QOS_OUI_TYPE)
		return -1;
	if (info_element->version != QOS_VERSION_1)
		return -1;

	return 0;
}