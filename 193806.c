static struct sc_card_driver *sc_get_driver(void)
{
	struct sc_card_driver *iso_drv = sc_get_iso7816_driver();

	setcos_ops = *iso_drv->ops;
	setcos_ops.match_card = setcos_match_card;
	setcos_ops.init = setcos_init;
	if (iso_ops == NULL)
		iso_ops = iso_drv->ops;
	setcos_ops.create_file = setcos_create_file;
	setcos_ops.set_security_env = setcos_set_security_env;
	setcos_ops.select_file = setcos_select_file;
	setcos_ops.list_files = setcos_list_files;
	setcos_ops.process_fci = setcos_process_fci;
	setcos_ops.construct_fci = setcos_construct_fci;
	setcos_ops.card_ctl = setcos_card_ctl;

	return &setcos_drv;
}