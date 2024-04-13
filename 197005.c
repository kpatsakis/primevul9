static void init_params_sja1000(struct ems_cpc_msg *msg)
{
	struct cpc_sja1000_params *sja1000 =
		&msg->msg.can_params.cc_params.sja1000;

	msg->type = CPC_CMD_TYPE_CAN_PARAMS;
	msg->length = sizeof(struct cpc_can_params);
	msg->msgid = 0;

	msg->msg.can_params.cc_type = CPC_CC_TYPE_SJA1000;

	/* Acceptance filter open */
	sja1000->acc_code0 = 0x00;
	sja1000->acc_code1 = 0x00;
	sja1000->acc_code2 = 0x00;
	sja1000->acc_code3 = 0x00;

	/* Acceptance filter open */
	sja1000->acc_mask0 = 0xFF;
	sja1000->acc_mask1 = 0xFF;
	sja1000->acc_mask2 = 0xFF;
	sja1000->acc_mask3 = 0xFF;

	sja1000->btr0 = 0;
	sja1000->btr1 = 0;

	sja1000->outp_contr = SJA1000_DEFAULT_OUTPUT_CONTROL;
	sja1000->mode = SJA1000_MOD_RM;
}