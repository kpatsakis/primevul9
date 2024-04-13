static int rtw_wx_set_enc(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *keybuf)
{
	u32 key, ret = 0;
	u32 keyindex_provided;
	struct ndis_802_11_wep	 wep;
	enum ndis_802_11_auth_mode authmode;

	struct iw_point *erq = &wrqu->encoding;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrpriv = &padapter->pwrctrlpriv;

	DBG_88E("+%s, flags = 0x%x\n", __func__, erq->flags);

	memset(&wep, 0, sizeof(struct ndis_802_11_wep));

	key = erq->flags & IW_ENCODE_INDEX;

	if (erq->flags & IW_ENCODE_DISABLED) {
		DBG_88E("EncryptionDisabled\n");
		padapter->securitypriv.ndisencryptstatus = Ndis802_11EncryptionDisabled;
		padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		padapter->securitypriv.dot118021XGrpPrivacy = _NO_PRIVACY_;
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
		authmode = Ndis802_11AuthModeOpen;
		padapter->securitypriv.ndisauthtype = authmode;

		goto exit;
	}

	if (key) {
		if (key > WEP_KEYS)
			return -EINVAL;
		key--;
		keyindex_provided = 1;
	} else {
		keyindex_provided = 0;
		key = padapter->securitypriv.dot11PrivacyKeyIndex;
		DBG_88E("%s, key =%d\n", __func__, key);
	}

	/* set authentication mode */
	if (erq->flags & IW_ENCODE_OPEN) {
		DBG_88E("%s():IW_ENCODE_OPEN\n", __func__);
		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;/* Ndis802_11EncryptionDisabled; */
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
		padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		padapter->securitypriv.dot118021XGrpPrivacy = _NO_PRIVACY_;
		authmode = Ndis802_11AuthModeOpen;
		padapter->securitypriv.ndisauthtype = authmode;
	} else if (erq->flags & IW_ENCODE_RESTRICTED) {
		DBG_88E("%s():IW_ENCODE_RESTRICTED\n", __func__);
		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
		padapter->securitypriv.dot11PrivacyAlgrthm = _WEP40_;
		padapter->securitypriv.dot118021XGrpPrivacy = _WEP40_;
		authmode = Ndis802_11AuthModeShared;
		padapter->securitypriv.ndisauthtype = authmode;
	} else {
		DBG_88E("%s():erq->flags = 0x%x\n", __func__, erq->flags);

		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;/* Ndis802_11EncryptionDisabled; */
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
		padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
		padapter->securitypriv.dot118021XGrpPrivacy = _NO_PRIVACY_;
		authmode = Ndis802_11AuthModeOpen;
		padapter->securitypriv.ndisauthtype = authmode;
	}

	wep.KeyIndex = key;
	if (erq->length > 0) {
		wep.KeyLength = erq->length <= 5 ? 5 : 13;

		wep.Length = wep.KeyLength + offsetof(struct ndis_802_11_wep, KeyMaterial);
	} else {
		wep.KeyLength = 0;

		if (keyindex_provided == 1) {
			/*  set key_id only, no given KeyMaterial(erq->length == 0). */
			padapter->securitypriv.dot11PrivacyKeyIndex = key;

			DBG_88E("(keyindex_provided == 1), keyid =%d, key_len =%d\n", key, padapter->securitypriv.dot11DefKeylen[key]);

			switch (padapter->securitypriv.dot11DefKeylen[key]) {
			case 5:
				padapter->securitypriv.dot11PrivacyAlgrthm = _WEP40_;
				break;
			case 13:
				padapter->securitypriv.dot11PrivacyAlgrthm = _WEP104_;
				break;
			default:
				padapter->securitypriv.dot11PrivacyAlgrthm = _NO_PRIVACY_;
				break;
			}

			goto exit;
		}
	}

	wep.KeyIndex |= 0x80000000;

	memcpy(wep.KeyMaterial, keybuf, wep.KeyLength);

	if (!rtw_set_802_11_add_wep(padapter, &wep)) {
		if (rf_on == pwrpriv->rf_pwrstate)
			ret = -EOPNOTSUPP;
		goto exit;
	}

exit:
	return ret;
}