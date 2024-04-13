get_pw_flags (NMSettingVpn *s_vpn, const char *secret_name, const char *type_name)
{
	const char *val;
	NMSettingSecretFlags flags = NM_SETTING_SECRET_FLAG_NONE;

	/* Try new flags value first */
	if (nm_setting_get_secret_flags (NM_SETTING (s_vpn), secret_name, &flags, NULL))
		return flags;

	/* Otherwise try old "password type" value */
	val = nm_setting_vpn_get_data_item (s_vpn, type_name);
	if (val) {
		if (g_strcmp0 (val, NM_VPNC_PW_TYPE_ASK) == 0)
			return NM_SETTING_SECRET_FLAG_NOT_SAVED;
		else if (g_strcmp0 (val, NM_VPNC_PW_TYPE_UNUSED) == 0)
			return NM_SETTING_SECRET_FLAG_NOT_REQUIRED;

		/* NM_VPNC_PW_TYPE_SAVE means FLAG_NONE */
	}

	return NM_SETTING_SECRET_FLAG_NONE;
}