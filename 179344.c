static int rtw_set_encryption(struct net_device *dev, struct ieee_param *param, u32 param_len)
{
	int ret = 0;
	u32 wep_key_idx, wep_key_len, wep_total_len;
	struct ndis_802_11_wep	 *pwep = NULL;
	struct sta_info *psta = NULL, *pbcmc_sta = NULL;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct sta_priv *pstapriv = &padapter->stapriv;

	DBG_88E("%s\n", __func__);
	param->u.crypt.err = 0;
	param->u.crypt.alg[IEEE_CRYPT_ALG_NAME_LEN - 1] = '\0';
	if (param_len !=  sizeof(struct ieee_param) + param->u.crypt.key_len) {
		ret =  -EINVAL;
		goto exit;
	}
	if (is_broadcast_ether_addr(param->sta_addr)) {
		if (param->u.crypt.idx >= WEP_KEYS) {
			ret = -EINVAL;
			goto exit;
		}
	} else {
		psta = rtw_get_stainfo(pstapriv, param->sta_addr);
		if (!psta) {
			DBG_88E("%s(), sta has already been removed or never been added\n", __func__);
			goto exit;
		}
	}

	if (strcmp(param->u.crypt.alg, "none") == 0 && (!psta)) {
		/* todo:clear default encryption keys */

		DBG_88E("clear default encryption keys, keyid =%d\n", param->u.crypt.idx);
		goto exit;
	}
	if (strcmp(param->u.crypt.alg, "WEP") == 0 && (!psta)) {
		DBG_88E("r871x_set_encryption, crypt.alg = WEP\n");
		wep_key_idx = param->u.crypt.idx;
		wep_key_len = param->u.crypt.key_len;
		DBG_88E("r871x_set_encryption, wep_key_idx=%d, len=%d\n", wep_key_idx, wep_key_len);
		if ((wep_key_idx >= WEP_KEYS) || (wep_key_len <= 0)) {
			ret = -EINVAL;
			goto exit;
		}

		if (wep_key_len > 0) {
			wep_key_len = wep_key_len <= 5 ? 5 : 13;
			wep_total_len = wep_key_len + offsetof(struct ndis_802_11_wep, KeyMaterial);
			pwep = (struct ndis_802_11_wep *)rtw_malloc(wep_total_len);
			if (!pwep) {
				DBG_88E(" r871x_set_encryption: pwep allocate fail !!!\n");
				goto exit;
			}

			memset(pwep, 0, wep_total_len);

			pwep->KeyLength = wep_key_len;
			pwep->Length = wep_total_len;
		}

		pwep->KeyIndex = wep_key_idx;

		memcpy(pwep->KeyMaterial,  param->u.crypt.key, pwep->KeyLength);

		if (param->u.crypt.set_tx) {
			DBG_88E("wep, set_tx = 1\n");

			psecuritypriv->ndisencryptstatus = Ndis802_11Encryption1Enabled;
			psecuritypriv->dot11PrivacyAlgrthm = _WEP40_;
			psecuritypriv->dot118021XGrpPrivacy = _WEP40_;

			if (pwep->KeyLength == 13) {
				psecuritypriv->dot11PrivacyAlgrthm = _WEP104_;
				psecuritypriv->dot118021XGrpPrivacy = _WEP104_;
			}

			psecuritypriv->dot11PrivacyKeyIndex = wep_key_idx;

			memcpy(&psecuritypriv->dot11DefKey[wep_key_idx].skey[0], pwep->KeyMaterial, pwep->KeyLength);

			psecuritypriv->dot11DefKeylen[wep_key_idx] = pwep->KeyLength;

			set_wep_key(padapter, pwep->KeyMaterial, pwep->KeyLength, wep_key_idx);
		} else {
			DBG_88E("wep, set_tx = 0\n");

			/* don't update "psecuritypriv->dot11PrivacyAlgrthm" and */
			/* psecuritypriv->dot11PrivacyKeyIndex = keyid", but can rtw_set_key to cam */

			memcpy(&psecuritypriv->dot11DefKey[wep_key_idx].skey[0], pwep->KeyMaterial, pwep->KeyLength);

			psecuritypriv->dot11DefKeylen[wep_key_idx] = pwep->KeyLength;

			set_wep_key(padapter, pwep->KeyMaterial, pwep->KeyLength, wep_key_idx);
		}

		goto exit;
	}

	if (!psta && check_fwstate(pmlmepriv, WIFI_AP_STATE)) { /*  group key */
		if (param->u.crypt.set_tx == 1) {
			if (strcmp(param->u.crypt.alg, "WEP") == 0) {
				DBG_88E("%s, set group_key, WEP\n", __func__);

				memcpy(psecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,
				       param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));

				psecuritypriv->dot118021XGrpPrivacy = _WEP40_;
				if (param->u.crypt.key_len == 13)
					psecuritypriv->dot118021XGrpPrivacy = _WEP104_;
			} else if (strcmp(param->u.crypt.alg, "TKIP") == 0) {
				DBG_88E("%s, set group_key, TKIP\n", __func__);
				psecuritypriv->dot118021XGrpPrivacy = _TKIP_;
				memcpy(psecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,
				       param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));
				/* set mic key */
				memcpy(psecuritypriv->dot118021XGrptxmickey[param->u.crypt.idx].skey, &param->u.crypt.key[16], 8);
				memcpy(psecuritypriv->dot118021XGrprxmickey[param->u.crypt.idx].skey, &param->u.crypt.key[24], 8);

				psecuritypriv->busetkipkey = true;
			} else if (strcmp(param->u.crypt.alg, "CCMP") == 0) {
				DBG_88E("%s, set group_key, CCMP\n", __func__);
				psecuritypriv->dot118021XGrpPrivacy = _AES_;
				memcpy(psecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,
				       param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));
			} else {
				DBG_88E("%s, set group_key, none\n", __func__);
				psecuritypriv->dot118021XGrpPrivacy = _NO_PRIVACY_;
			}
			psecuritypriv->dot118021XGrpKeyid = param->u.crypt.idx;
			psecuritypriv->binstallGrpkey = true;
			psecuritypriv->dot11PrivacyAlgrthm = psecuritypriv->dot118021XGrpPrivacy;/*  */
			set_group_key(padapter, param->u.crypt.key, psecuritypriv->dot118021XGrpPrivacy, param->u.crypt.idx);
			pbcmc_sta = rtw_get_bcmc_stainfo(padapter);
			if (pbcmc_sta) {
				pbcmc_sta->ieee8021x_blocked = false;
				pbcmc_sta->dot118021XPrivacy = psecuritypriv->dot118021XGrpPrivacy;/* rx will use bmc_sta's dot118021XPrivacy */
			}
		}
		goto exit;
	}

	if (psecuritypriv->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X && psta) { /*  psk/802_1x */
		if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
			if (param->u.crypt.set_tx == 1) {
				memcpy(psta->dot118021x_UncstKey.skey,  param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));

				if (strcmp(param->u.crypt.alg, "WEP") == 0) {
					DBG_88E("%s, set pairwise key, WEP\n", __func__);

					psta->dot118021XPrivacy = _WEP40_;
					if (param->u.crypt.key_len == 13)
						psta->dot118021XPrivacy = _WEP104_;
				} else if (strcmp(param->u.crypt.alg, "TKIP") == 0) {
					DBG_88E("%s, set pairwise key, TKIP\n", __func__);

					psta->dot118021XPrivacy = _TKIP_;

					/* set mic key */
					memcpy(psta->dot11tkiptxmickey.skey, &param->u.crypt.key[16], 8);
					memcpy(psta->dot11tkiprxmickey.skey, &param->u.crypt.key[24], 8);

					psecuritypriv->busetkipkey = true;
				} else if (strcmp(param->u.crypt.alg, "CCMP") == 0) {
					DBG_88E("%s, set pairwise key, CCMP\n", __func__);

					psta->dot118021XPrivacy = _AES_;
				} else {
					DBG_88E("%s, set pairwise key, none\n", __func__);

					psta->dot118021XPrivacy = _NO_PRIVACY_;
				}

				set_pairwise_key(padapter, psta);

				psta->ieee8021x_blocked = false;
			} else { /* group key??? */
				if (strcmp(param->u.crypt.alg, "WEP") == 0) {
					memcpy(psecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,
					       param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));
					psecuritypriv->dot118021XGrpPrivacy = _WEP40_;
					if (param->u.crypt.key_len == 13)
						psecuritypriv->dot118021XGrpPrivacy = _WEP104_;
				} else if (strcmp(param->u.crypt.alg, "TKIP") == 0) {
					psecuritypriv->dot118021XGrpPrivacy = _TKIP_;

					memcpy(psecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,
					       param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));

					/* set mic key */
					memcpy(psecuritypriv->dot118021XGrptxmickey[param->u.crypt.idx].skey, &param->u.crypt.key[16], 8);
					memcpy(psecuritypriv->dot118021XGrprxmickey[param->u.crypt.idx].skey, &param->u.crypt.key[24], 8);

					psecuritypriv->busetkipkey = true;
				} else if (strcmp(param->u.crypt.alg, "CCMP") == 0) {
					psecuritypriv->dot118021XGrpPrivacy = _AES_;

					memcpy(psecuritypriv->dot118021XGrpKey[param->u.crypt.idx].skey,
					       param->u.crypt.key, min_t(u16, param->u.crypt.key_len, 16));
				} else {
					psecuritypriv->dot118021XGrpPrivacy = _NO_PRIVACY_;
				}

				psecuritypriv->dot118021XGrpKeyid = param->u.crypt.idx;

				psecuritypriv->binstallGrpkey = true;

				psecuritypriv->dot11PrivacyAlgrthm = psecuritypriv->dot118021XGrpPrivacy;/*  */

				set_group_key(padapter, param->u.crypt.key, psecuritypriv->dot118021XGrpPrivacy, param->u.crypt.idx);

				pbcmc_sta = rtw_get_bcmc_stainfo(padapter);
				if (pbcmc_sta) {
					pbcmc_sta->ieee8021x_blocked = false;
					pbcmc_sta->dot118021XPrivacy = psecuritypriv->dot118021XGrpPrivacy;/* rx will use bmc_sta's dot118021XPrivacy */
				}
			}
		}
	}

exit:

	kfree(pwep);

	return ret;
}