static struct sc_card_driver * sc_get_driver(void)
{
	struct sc_card_driver *iso_drv = sc_get_iso7816_driver();

	cac_ops = *iso_drv->ops;
	cac_ops.match_card = cac_match_card;
	cac_ops.init = cac_init;
	cac_ops.finish = cac_finish;

	cac_ops.select_file =  cac_select_file; /* need to record object type */
	cac_ops.get_challenge = cac_get_challenge;
	cac_ops.read_binary = cac_read_binary;
	cac_ops.write_binary = cac_write_binary;
	cac_ops.set_security_env = cac_set_security_env;
	cac_ops.restore_security_env = cac_restore_security_env;
	cac_ops.compute_signature = cac_compute_signature;
	cac_ops.decipher =  cac_decipher;
	cac_ops.card_ctl = cac_card_ctl;
	cac_ops.pin_cmd = cac_pin_cmd;

	return &cac_drv;
}