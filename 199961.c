static int ieee80211_parse_qos_info_param_IE(struct ieee80211_info_element
					     *info_element,
					     struct ieee80211_network *network)
{
	int rc = 0;
	struct ieee80211_qos_parameters *qos_param = NULL;
	struct ieee80211_qos_information_element qos_info_element;

	rc = ieee80211_read_qos_info_element(&qos_info_element, info_element);

	if (rc == 0) {
		network->qos_data.param_count = qos_info_element.ac_info & 0x0F;
		network->flags |= NETWORK_HAS_QOS_INFORMATION;
	} else {
		struct ieee80211_qos_parameter_info param_element;

		rc = ieee80211_read_qos_param_element(&param_element,
						      info_element);
		if (rc == 0) {
			qos_param = &(network->qos_data.parameters);
			ieee80211_qos_convert_ac_to_parameters(&param_element,
							       qos_param);
			network->flags |= NETWORK_HAS_QOS_PARAMETERS;
			network->qos_data.param_count =
			    param_element.info_element.ac_info & 0x0F;
		}
	}

	if (rc == 0) {
		IEEE80211_DEBUG_QOS("QoS is supported\n");
		network->qos_data.supported = 1;
	}
	return rc;
}