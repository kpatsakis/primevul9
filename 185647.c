mwifiex_wmm_convert_tos_to_ac(struct mwifiex_adapter *adapter, u32 tos)
{
	/* Map of TOS UP values to WMM AC */
	static const enum mwifiex_wmm_ac_e tos_to_ac[] = {
		WMM_AC_BE,
		WMM_AC_BK,
		WMM_AC_BK,
		WMM_AC_BE,
		WMM_AC_VI,
		WMM_AC_VI,
		WMM_AC_VO,
		WMM_AC_VO
	};

	if (tos >= ARRAY_SIZE(tos_to_ac))
		return WMM_AC_BE;

	return tos_to_ac[tos];
}