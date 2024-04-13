struct sc_card_driver * sc_get_tcos_driver(void)
{
	struct sc_card_driver *iso_drv = sc_get_iso7816_driver();

	if (iso_ops == NULL) iso_ops = iso_drv->ops;
	tcos_ops = *iso_drv->ops;

	tcos_ops.match_card           = tcos_match_card;
	tcos_ops.init                 = tcos_init;
	tcos_ops.finish               = tcos_finish;
	tcos_ops.create_file          = tcos_create_file;
	tcos_ops.set_security_env     = tcos_set_security_env;
	tcos_ops.select_file          = tcos_select_file;
	tcos_ops.list_files           = tcos_list_files;
	tcos_ops.delete_file          = tcos_delete_file;
	tcos_ops.set_security_env     = tcos_set_security_env;
	tcos_ops.compute_signature    = tcos_compute_signature;
	tcos_ops.decipher             = tcos_decipher;
	tcos_ops.restore_security_env = tcos_restore_security_env;
	tcos_ops.card_ctl             = tcos_card_ctl;
	
	return &tcos_drv;
}