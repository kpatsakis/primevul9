static int rtw_wx_set_pmkid(struct net_device *dev,
			    struct iw_request_info *a,
			    union iwreq_data *wrqu, char *extra)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	u8   j, blInserted = false;
	int  ret = false;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct iw_pmksa *pPMK = (struct iw_pmksa *)extra;
	u8     strZeroMacAddress[ETH_ALEN] = {0x00};
	u8     strIssueBssid[ETH_ALEN] = {0x00};

	memcpy(strIssueBssid, pPMK->bssid.sa_data, ETH_ALEN);
	if (pPMK->cmd == IW_PMKSA_ADD) {
		DBG_88E("[%s] IW_PMKSA_ADD!\n", __func__);
		if (!memcmp(strIssueBssid, strZeroMacAddress, ETH_ALEN))
			return ret;
		ret = true;
		blInserted = false;

		/* overwrite PMKID */
		for (j = 0; j < NUM_PMKID_CACHE; j++) {
			if (!memcmp(psecuritypriv->PMKIDList[j].bssid, strIssueBssid, ETH_ALEN)) {
				/*  BSSID is matched, the same AP => rewrite with new PMKID. */
				DBG_88E("[%s] BSSID exists in the PMKList.\n", __func__);
				memcpy(psecuritypriv->PMKIDList[j].PMKID, pPMK->pmkid, IW_PMKID_LEN);
				psecuritypriv->PMKIDList[j].used = true;
				psecuritypriv->PMKIDIndex = j + 1;
				blInserted = true;
				break;
			}
		}

		if (!blInserted) {
			/*  Find a new entry */
			DBG_88E("[%s] Use the new entry index = %d for this PMKID.\n",
				__func__, psecuritypriv->PMKIDIndex);

			memcpy(psecuritypriv->PMKIDList[psecuritypriv->PMKIDIndex].bssid, strIssueBssid, ETH_ALEN);
			memcpy(psecuritypriv->PMKIDList[psecuritypriv->PMKIDIndex].PMKID, pPMK->pmkid, IW_PMKID_LEN);

			psecuritypriv->PMKIDList[psecuritypriv->PMKIDIndex].used = true;
			psecuritypriv->PMKIDIndex++;
			if (psecuritypriv->PMKIDIndex == 16)
				psecuritypriv->PMKIDIndex = 0;
		}
	} else if (pPMK->cmd == IW_PMKSA_REMOVE) {
		DBG_88E("[%s] IW_PMKSA_REMOVE!\n", __func__);
		ret = true;
		for (j = 0; j < NUM_PMKID_CACHE; j++) {
			if (!memcmp(psecuritypriv->PMKIDList[j].bssid, strIssueBssid, ETH_ALEN)) {
				/*  BSSID is matched, the same AP => Remove this PMKID information and reset it. */
				eth_zero_addr(psecuritypriv->PMKIDList[j].bssid);
				psecuritypriv->PMKIDList[j].used = false;
				break;
			}
		}
	} else if (pPMK->cmd == IW_PMKSA_FLUSH) {
		DBG_88E("[%s] IW_PMKSA_FLUSH!\n", __func__);
		memset(&psecuritypriv->PMKIDList[0], 0x00, sizeof(struct rt_pmkid_list) * NUM_PMKID_CACHE);
		psecuritypriv->PMKIDIndex = 0;
		ret = true;
	}
	return ret;
}