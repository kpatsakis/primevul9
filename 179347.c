static int wpa_set_encryption(struct net_device *dev, struct ieee_param *param, u32 param_len)
{
	int ret = 0;
	u32 wep_key_idx, wep_key_len, wep_total_len;
	struct ndis_802_11_wep *pwep = NULL;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;

	param->u.crypt.err = 0;
	param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';

	if (param_len < (u32)((u8 *)param->u.crypt.key - (u8 *)param) + param->u.crypt.key_len) {
		ret =  -EINVAL;
		goto exit;
	}

	if (is_broadcast_ether_addr(param->sta_addr)) {
		if (param->u.crypt.idx >= WEP_KEYS) {
			ret = -EINVAL;
			goto exit;
		}
	} else {
		ret = -EINVAL;
		goto exit;
	}

	if (strcmp(param->u.crypt.alg, "WEP") == 0) {
		RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_err_, ("%s, crypt.alg = WEP\n", __func__));
		DBG_88E("%s, crypt.alg = WEP\n", __func__);

		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;
		padapter->securitypriv.dot11PrivacyAlgrthm = _WEP40_;
		padapter->securitypriv.dot118021XGrpPrivacy = _WEP40_;

		wep_key_idx = param->u.crypt.idx;
		wep_key_len = param->u.crypt.key_len;

		RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_, ("(1)wep_key_idx =%d\n", wep_key_idx));
		DBG_88E("(1)wep_key_idx =%d\n", wep_key_idx);

		if (wep_key_idx > WEP_KEYS)
			return -EINVAL;

		RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_, ("(2)wep_key_idx =%d\n", wep_key_idx));

		if (wep_key_len > 0) {
			wep_key_len = wep_key_len <= 5 ? 5 : 13;
			wep_total_len = wep_key_len + offsetof(struct ndis_802_11_wep, KeyMaterial);
			pwep = (struct ndis_802_11_wep *)rtw_malloc(wep_total_len);
			if (!pwep) {
				RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_err_, ("%s: pwep allocate fail !!!\n", __func__));
				goto exit;
			}
			memset(pwep, 0, wep_total_len);
			pwep->KeyLength = wep_key_len;
			pwep->Length = wep_total_len;
			if (wep_key_len == 13) {
				padapter->securitypriv.dot11PrivacyAlgrthm = _WEP104_;
				padapter->securitypriv.dot118021XGrpPrivacy = _WEP104_;
			}
		} else {
			ret = -EINVAL;
			goto exit;
		}
		pwep->KeyIndex = wep_key_idx;
		pwep->KeyIndex |= 0x80000000;
		memcpy(pwep->KeyMaterial,  param->u.crypt.key, pwep->KeyLength);
		if (param->u.crypt.set_tx) {
			DBG_88E("wep, set_tx = 1\n");
			if (rtw_set_802_11_add_wep(padapter, pwep) == (u8)_FAIL)
				ret = -EOPNOTSUPP;
		} else {
			DBG_88E("wep, set_tx = 0\n");
			if (wep_key_idx >= WEP_KEYS) {
				ret = -EOPNOTSUPP;
				goto exit;
			}
			memcpy(&psecuritypriv->dot11DefKey[wep_key_idx].skey[0], pwep->KeyMaterial, pwep->KeyLength);
			psecuritypriv->dot11DefKeylen[wep_key_idx] = pwep->KeyLength;
			rtw_set_key(padapter, psecuritypriv, wep_key_idx, 0);
		}
		goto exit;
	}

	if (padapter->securitypriv.dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) { /*  802_1x */
		struct sta_info *psta, *pbcmc_sta;
		struct sta_priv *pstapriv = &padapter->stapriv;

		if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) { /* sta mode */
			psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
			if (!psta) {
				;
			} else {
				if (strcmp(param->u.crypt.alg, "none") != 0)
					psta->ieee8021x_blocked = false;

				if ((padapter->securitypriv.ndisencryptstatus == Ndis802_11Encryption2Enabled) ||
				    (padapter->securitypriv.ndisencryptstatus == Ndis802_11Encryption3Enabled))
					psta->dot118021XPrivacy = padapter->securitypriv.dot11PrivacyAlgrthm;

				if (param->u.crypt.set_tx == 1) { /* pairwise key */
					memcpy(psta->dot118021x_UncstKey.skey, param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));

					if (strcmp(param->u.crypt.alg, "TKIP") == 0) { /* set mic key */
						memcpy(psta->dot11tkiptxmickey.skey, &param->u.crypt.key[16], 8);
						memcpy(psta->dot11tkiprxmickey.skey, &param->u.crypt.key[24], 8);
						padapter->securitypriv.busetkipkey = false;
					}

					DBG_88E(" ~~~~set sta key:unicastkey\n");

					rtw_setstakey_cmd(padapter, (unsigned char *)psta, true);
				} else { /* group key */
					memcpy(padapter->securitypriv.dot118021XGrpKey[param->u.crypt.idx].skey, param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));
					memcpy(padapter->securitypriv.dot118021XGrptxmickey[param->u.crypt.idx].skey, &param->u.crypt.key[16], 8);
					memcpy(padapter->securitypriv.dot118021XGrprxmickey[param->u.crypt.idx].skey, &param->u.crypt.key[24], 8);
					padapter->securitypriv.binstallGrpkey = true;
					DBG_88E(" ~~~~set sta key:groupkey\n");

					padapter->securitypriv.dot118021XGrpKeyid = param->u.crypt.idx;

					rtw_set_key(padapter, &padapter->securitypriv, param->u.crypt.idx, 1);
				}
			}
			pbcmc_sta = rtw_get_bcmc_stainfo(padapter);
			if (!pbcmc_sta) {
				;
			} else {
				/* Jeff: don't disable ieee8021x_blocked while clearing key */
				if (strcmp(param->u.crypt.alg, "none") != 0)
					pbcmc_sta->ieee8021x_blocked = false;

				if ((padapter->securitypriv.ndisencryptstatus == Ndis802_11Encryption2Enabled) ||
				    (padapter->securitypriv.ndisencryptstatus == Ndis802_11Encryption3Enabled))
					pbcmc_sta->dot118021XPrivacy = padapter->securitypriv.dot11PrivacyAlgrthm;
			}
		}
	}

exit:

	kfree(pwep);
	return ret;
}