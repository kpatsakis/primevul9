static int ieee80211_parse_info_param(struct ieee80211_info_element
				      *info_element, u16 length,
				      struct ieee80211_network *network)
{
	u8 i;
#ifdef CONFIG_IEEE80211_DEBUG
	char rates_str[64];
	char *p;
#endif

	while (length >= sizeof(*info_element)) {
		if (sizeof(*info_element) + info_element->len > length) {
			IEEE80211_DEBUG_MGMT("Info elem: parse failed: "
					     "info_element->len + 2 > left : "
					     "info_element->len+2=%zd left=%d, id=%d.\n",
					     info_element->len +
					     sizeof(*info_element),
					     length, info_element->id);
			/* We stop processing but don't return an error here
			 * because some misbehaviour APs break this rule. ie.
			 * Orinoco AP1000. */
			break;
		}

		switch (info_element->id) {
		case MFIE_TYPE_SSID:
			if (ieee80211_is_empty_essid(info_element->data,
						     info_element->len)) {
				network->flags |= NETWORK_EMPTY_ESSID;
				break;
			}

			network->ssid_len = min(info_element->len,
						(u8) IW_ESSID_MAX_SIZE);
			memcpy(network->ssid, info_element->data,
			       network->ssid_len);
			if (network->ssid_len < IW_ESSID_MAX_SIZE)
				memset(network->ssid + network->ssid_len, 0,
				       IW_ESSID_MAX_SIZE - network->ssid_len);

			IEEE80211_DEBUG_MGMT("MFIE_TYPE_SSID: '%s' len=%d.\n",
					     network->ssid, network->ssid_len);
			break;

		case MFIE_TYPE_RATES:
#ifdef CONFIG_IEEE80211_DEBUG
			p = rates_str;
#endif
			network->rates_len = min(info_element->len,
						 MAX_RATES_LENGTH);
			for (i = 0; i < network->rates_len; i++) {
				network->rates[i] = info_element->data[i];
#ifdef CONFIG_IEEE80211_DEBUG
				p += snprintf(p, sizeof(rates_str) -
					      (p - rates_str), "%02X ",
					      network->rates[i]);
#endif
				if (ieee80211_is_ofdm_rate
				    (info_element->data[i])) {
					network->flags |= NETWORK_HAS_OFDM;
					if (info_element->data[i] &
					    IEEE80211_BASIC_RATE_MASK)
						network->flags &=
						    ~NETWORK_HAS_CCK;
				}
			}

			IEEE80211_DEBUG_MGMT("MFIE_TYPE_RATES: '%s' (%d)\n",
					     rates_str, network->rates_len);
			break;

		case MFIE_TYPE_RATES_EX:
#ifdef CONFIG_IEEE80211_DEBUG
			p = rates_str;
#endif
			network->rates_ex_len = min(info_element->len,
						    MAX_RATES_EX_LENGTH);
			for (i = 0; i < network->rates_ex_len; i++) {
				network->rates_ex[i] = info_element->data[i];
#ifdef CONFIG_IEEE80211_DEBUG
				p += snprintf(p, sizeof(rates_str) -
					      (p - rates_str), "%02X ",
					      network->rates[i]);
#endif
				if (ieee80211_is_ofdm_rate
				    (info_element->data[i])) {
					network->flags |= NETWORK_HAS_OFDM;
					if (info_element->data[i] &
					    IEEE80211_BASIC_RATE_MASK)
						network->flags &=
						    ~NETWORK_HAS_CCK;
				}
			}

			IEEE80211_DEBUG_MGMT("MFIE_TYPE_RATES_EX: '%s' (%d)\n",
					     rates_str, network->rates_ex_len);
			break;

		case MFIE_TYPE_DS_SET:
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_DS_SET: %d\n",
					     info_element->data[0]);
			network->channel = info_element->data[0];
			break;

		case MFIE_TYPE_FH_SET:
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_FH_SET: ignored\n");
			break;

		case MFIE_TYPE_CF_SET:
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_CF_SET: ignored\n");
			break;

		case MFIE_TYPE_TIM:
			network->tim.tim_count = info_element->data[0];
			network->tim.tim_period = info_element->data[1];
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_TIM: partially ignored\n");
			break;

		case MFIE_TYPE_ERP_INFO:
			network->erp_value = info_element->data[0];
			network->flags |= NETWORK_HAS_ERP_VALUE;
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_ERP_SET: %d\n",
					     network->erp_value);
			break;

		case MFIE_TYPE_IBSS_SET:
			network->atim_window = info_element->data[0];
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_IBSS_SET: %d\n",
					     network->atim_window);
			break;

		case MFIE_TYPE_CHALLENGE:
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_CHALLENGE: ignored\n");
			break;

		case MFIE_TYPE_GENERIC:
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_GENERIC: %d bytes\n",
					     info_element->len);
			if (!ieee80211_parse_qos_info_param_IE(info_element,
							       network))
				break;

			if (info_element->len >= 4 &&
			    info_element->data[0] == 0x00 &&
			    info_element->data[1] == 0x50 &&
			    info_element->data[2] == 0xf2 &&
			    info_element->data[3] == 0x01) {
				network->wpa_ie_len = min(info_element->len + 2,
							  MAX_WPA_IE_LEN);
				memcpy(network->wpa_ie, info_element,
				       network->wpa_ie_len);
			}
			break;

		case MFIE_TYPE_RSN:
			IEEE80211_DEBUG_MGMT("MFIE_TYPE_RSN: %d bytes\n",
					     info_element->len);
			network->rsn_ie_len = min(info_element->len + 2,
						  MAX_WPA_IE_LEN);
			memcpy(network->rsn_ie, info_element,
			       network->rsn_ie_len);
			break;

		case MFIE_TYPE_QOS_PARAMETER:
			printk(KERN_ERR
			       "QoS Error need to parse QOS_PARAMETER IE\n");
			break;
			/* 802.11h */
		case MFIE_TYPE_POWER_CONSTRAINT:
			network->power_constraint = info_element->data[0];
			network->flags |= NETWORK_HAS_POWER_CONSTRAINT;
			break;

		case MFIE_TYPE_CSA:
			network->power_constraint = info_element->data[0];
			network->flags |= NETWORK_HAS_CSA;
			break;

		case MFIE_TYPE_QUIET:
			network->quiet.count = info_element->data[0];
			network->quiet.period = info_element->data[1];
			network->quiet.duration = info_element->data[2];
			network->quiet.offset = info_element->data[3];
			network->flags |= NETWORK_HAS_QUIET;
			break;

		case MFIE_TYPE_IBSS_DFS:
			if (network->ibss_dfs)
				break;
			network->ibss_dfs = kmemdup(info_element->data,
						    info_element->len,
						    GFP_ATOMIC);
			if (!network->ibss_dfs)
				return 1;
			network->flags |= NETWORK_HAS_IBSS_DFS;
			break;

		case MFIE_TYPE_TPC_REPORT:
			network->tpc_report.transmit_power =
			    info_element->data[0];
			network->tpc_report.link_margin = info_element->data[1];
			network->flags |= NETWORK_HAS_TPC_REPORT;
			break;

		default:
			IEEE80211_DEBUG_MGMT
			    ("Unsupported info element: %s (%d)\n",
			     get_info_element_string(info_element->id),
			     info_element->id);
			break;
		}

		length -= sizeof(*info_element) + info_element->len;
		info_element =
		    (struct ieee80211_info_element *)&info_element->
		    data[info_element->len];
	}

	return 0;
}