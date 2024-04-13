static struct sc_card_driver * sc_get_driver(void)
{
	struct sc_card_driver *iso_drv = sc_get_iso7816_driver();

	coolkey_ops = *iso_drv->ops;
	coolkey_ops.match_card = coolkey_match_card;
	coolkey_ops.init = coolkey_init;
	coolkey_ops.finish = coolkey_finish;

	coolkey_ops.select_file =  coolkey_select_file; /* need to record object type */
	coolkey_ops.get_challenge = coolkey_get_challenge;
	coolkey_ops.read_binary = coolkey_read_binary;
	coolkey_ops.write_binary = coolkey_write_binary;
	coolkey_ops.set_security_env = coolkey_set_security_env;
	coolkey_ops.restore_security_env = coolkey_restore_security_env;
	coolkey_ops.compute_signature = coolkey_compute_crypt;
	coolkey_ops.decipher =  coolkey_compute_crypt;
	coolkey_ops.card_ctl = coolkey_card_ctl;
	coolkey_ops.check_sw = coolkey_check_sw;
	coolkey_ops.pin_cmd = coolkey_pin_cmd;
	coolkey_ops.logout = coolkey_logout;
	coolkey_ops.card_reader_lock_obtained = coolkey_card_reader_lock_obtained;

	return &coolkey_drv;
}