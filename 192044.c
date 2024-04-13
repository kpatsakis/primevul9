static void rtl8xxxu_dump_action(struct device *dev,
				 struct ieee80211_hdr *hdr)
{
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)hdr;
	u16 cap, timeout;

	if (!(rtl8xxxu_debug & RTL8XXXU_DEBUG_ACTION))
		return;

	switch (mgmt->u.action.u.addba_resp.action_code) {
	case WLAN_ACTION_ADDBA_RESP:
		cap = le16_to_cpu(mgmt->u.action.u.addba_resp.capab);
		timeout = le16_to_cpu(mgmt->u.action.u.addba_resp.timeout);
		dev_info(dev, "WLAN_ACTION_ADDBA_RESP: "
			 "timeout %i, tid %02x, buf_size %02x, policy %02x, "
			 "status %02x\n",
			 timeout,
			 (cap & IEEE80211_ADDBA_PARAM_TID_MASK) >> 2,
			 (cap & IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK) >> 6,
			 (cap >> 1) & 0x1,
			 le16_to_cpu(mgmt->u.action.u.addba_resp.status));
		break;
	case WLAN_ACTION_ADDBA_REQ:
		cap = le16_to_cpu(mgmt->u.action.u.addba_req.capab);
		timeout = le16_to_cpu(mgmt->u.action.u.addba_req.timeout);
		dev_info(dev, "WLAN_ACTION_ADDBA_REQ: "
			 "timeout %i, tid %02x, buf_size %02x, policy %02x\n",
			 timeout,
			 (cap & IEEE80211_ADDBA_PARAM_TID_MASK) >> 2,
			 (cap & IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK) >> 6,
			 (cap >> 1) & 0x1);
		break;
	default:
		dev_info(dev, "action frame %02x\n",
			 mgmt->u.action.u.addba_resp.action_code);
		break;
	}
}