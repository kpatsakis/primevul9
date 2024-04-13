static int ieee80211_read_qos_param_element(struct ieee80211_qos_parameter_info
					    *element_param, struct ieee80211_info_element
					    *info_element)
{
	int ret = 0;
	u16 size = sizeof(struct ieee80211_qos_parameter_info) - 2;

	if ((info_element == NULL) || (element_param == NULL))
		return -1;

	if (info_element->id == QOS_ELEMENT_ID && info_element->len == size) {
		memcpy(element_param->info_element.qui, info_element->data,
		       info_element->len);
		element_param->info_element.elementID = info_element->id;
		element_param->info_element.length = info_element->len;
	} else
		ret = -1;
	if (ret == 0)
		ret = ieee80211_verify_qos_info(&element_param->info_element,
						QOS_OUI_PARAM_SUB_TYPE);
	return ret;
}