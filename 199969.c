static const char *get_info_element_string(u16 id)
{
	switch (id) {
		MFIE_STRING(SSID);
		MFIE_STRING(RATES);
		MFIE_STRING(FH_SET);
		MFIE_STRING(DS_SET);
		MFIE_STRING(CF_SET);
		MFIE_STRING(TIM);
		MFIE_STRING(IBSS_SET);
		MFIE_STRING(COUNTRY);
		MFIE_STRING(HOP_PARAMS);
		MFIE_STRING(HOP_TABLE);
		MFIE_STRING(REQUEST);
		MFIE_STRING(CHALLENGE);
		MFIE_STRING(POWER_CONSTRAINT);
		MFIE_STRING(POWER_CAPABILITY);
		MFIE_STRING(TPC_REQUEST);
		MFIE_STRING(TPC_REPORT);
		MFIE_STRING(SUPP_CHANNELS);
		MFIE_STRING(CSA);
		MFIE_STRING(MEASURE_REQUEST);
		MFIE_STRING(MEASURE_REPORT);
		MFIE_STRING(QUIET);
		MFIE_STRING(IBSS_DFS);
		MFIE_STRING(ERP_INFO);
		MFIE_STRING(RSN);
		MFIE_STRING(RATES_EX);
		MFIE_STRING(GENERIC);
		MFIE_STRING(QOS_PARAMETER);
	default:
		return "UNKNOWN";
	}
}