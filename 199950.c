static int ieee80211_qos_convert_ac_to_parameters(struct
						  ieee80211_qos_parameter_info
						  *param_elm, struct
						  ieee80211_qos_parameters
						  *qos_param)
{
	int rc = 0;
	int i;
	struct ieee80211_qos_ac_parameter *ac_params;
	u32 txop;
	u8 cw_min;
	u8 cw_max;

	for (i = 0; i < QOS_QUEUE_NUM; i++) {
		ac_params = &(param_elm->ac_params_record[i]);

		qos_param->aifs[i] = (ac_params->aci_aifsn) & 0x0F;
		qos_param->aifs[i] -= (qos_param->aifs[i] < 2) ? 0 : 2;

		cw_min = ac_params->ecw_min_max & 0x0F;
		qos_param->cw_min[i] = (u16) ((1 << cw_min) - 1);

		cw_max = (ac_params->ecw_min_max & 0xF0) >> 4;
		qos_param->cw_max[i] = (u16) ((1 << cw_max) - 1);

		qos_param->flag[i] =
		    (ac_params->aci_aifsn & 0x10) ? 0x01 : 0x00;

		txop = le16_to_cpu(ac_params->tx_op_limit) * 32;
		qos_param->tx_op_limit[i] = (u16) txop;
	}
	return rc;
}