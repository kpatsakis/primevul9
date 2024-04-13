static struct sc_card_driver * sc_get_driver(void)
{
	struct sc_card_driver *iso_drv = sc_get_iso7816_driver();
	if (iso_ops == NULL)
		iso_ops = iso_drv->ops;

	muscle_ops = *iso_drv->ops;
	muscle_ops.check_sw = muscle_check_sw;
	muscle_ops.pin_cmd = muscle_pin_cmd;
	muscle_ops.match_card = muscle_match_card;
	muscle_ops.init = muscle_init;
	muscle_ops.finish = muscle_finish;

	muscle_ops.get_challenge = muscle_get_challenge;

	muscle_ops.set_security_env = muscle_set_security_env;
	muscle_ops.restore_security_env = muscle_restore_security_env;
	muscle_ops.compute_signature = muscle_compute_signature;
	muscle_ops.decipher = muscle_decipher;
	muscle_ops.card_ctl = muscle_card_ctl;
	muscle_ops.read_binary = muscle_read_binary;
	muscle_ops.update_binary = muscle_update_binary;
	muscle_ops.create_file = muscle_create_file;
	muscle_ops.select_file = muscle_select_file;
	muscle_ops.delete_file = muscle_delete_file;
	muscle_ops.list_files = muscle_list_files;
	muscle_ops.card_reader_lock_obtained = muscle_card_reader_lock_obtained;

	return &muscle_drv;
}