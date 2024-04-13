static struct sc_card_driver *sc_get_driver(void)
{
	struct sc_card_driver *iso_drv = sc_get_iso7816_driver();

	if (iso_ops == NULL)
		iso_ops = iso_drv->ops;

	epass2003_ops = *iso_ops;

	epass2003_ops.match_card = epass2003_match_card;
	epass2003_ops.init = epass2003_init;
	epass2003_ops.finish = epass2003_finish;
	epass2003_ops.write_binary = NULL;
	epass2003_ops.write_record = NULL;
	epass2003_ops.select_file = epass2003_select_file;
	epass2003_ops.get_response = NULL;
	epass2003_ops.restore_security_env = epass2003_restore_security_env;
	epass2003_ops.set_security_env = epass2003_set_security_env;
	epass2003_ops.decipher = epass2003_decipher;
	epass2003_ops.compute_signature = epass2003_decipher;
	epass2003_ops.create_file = epass2003_create_file;
	epass2003_ops.delete_file = epass2003_delete_file;
	epass2003_ops.list_files = epass2003_list_files;
	epass2003_ops.card_ctl = epass2003_card_ctl;
	epass2003_ops.process_fci = epass2003_process_fci;
	epass2003_ops.construct_fci = epass2003_construct_fci;
	epass2003_ops.pin_cmd = epass2003_pin_cmd;
	epass2003_ops.check_sw = epass2003_check_sw;
	epass2003_ops.get_challenge = epass2003_get_challenge;
	return &epass2003_drv;
}