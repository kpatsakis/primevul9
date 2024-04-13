static int wpa_set_auth_algs(struct net_device *dev, u32 value)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	int ret = 0;

	if ((value & AUTH_ALG_SHARED_KEY) && (value & AUTH_ALG_OPEN_SYSTEM)) {
		DBG_88E("%s, AUTH_ALG_SHARED_KEY and  AUTH_ALG_OPEN_SYSTEM [value:0x%x]\n", __func__, value);
		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;
		padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeAutoSwitch;
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Auto;
	} else if (value & AUTH_ALG_SHARED_KEY) {
		DBG_88E("%s, AUTH_ALG_SHARED_KEY  [value:0x%x]\n", __func__, value);
		padapter->securitypriv.ndisencryptstatus = Ndis802_11Encryption1Enabled;

		padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeShared;
		padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Shared;
	} else if (value & AUTH_ALG_OPEN_SYSTEM) {
		DBG_88E("%s, AUTH_ALG_OPEN_SYSTEM\n", __func__);
		if (padapter->securitypriv.ndisauthtype < Ndis802_11AuthModeWPAPSK) {
			padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeOpen;
			padapter->securitypriv.dot11AuthAlgrthm = dot11AuthAlgrthm_Open;
		}
	} else if (value & AUTH_ALG_LEAP) {
		DBG_88E("%s, AUTH_ALG_LEAP\n", __func__);
	} else {
		DBG_88E("%s, error!\n", __func__);
		ret = -EINVAL;
	}
	return ret;
}