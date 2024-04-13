static int cac_card_ctl(sc_card_t *card, unsigned long cmd, void *ptr)
{
	cac_private_data_t * priv = CAC_DATA(card);

	LOG_FUNC_CALLED(card->ctx);
	sc_log(card->ctx, "cmd=%ld ptr=%p", cmd, ptr);

	if (priv == NULL) {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_INTERNAL);
	}
	switch(cmd) {
		case SC_CARDCTL_CAC_GET_ACA_PATH:
			return cac_get_ACA_path(card, (sc_path_t *) ptr);
		case SC_CARDCTL_GET_SERIALNR:
			return cac_get_serial_nr_from_CUID(card, (sc_serial_number_t *) ptr);
		case SC_CARDCTL_CAC_INIT_GET_GENERIC_OBJECTS:
			return cac_get_init_and_get_count(&priv->general_list, &priv->general_current, (int *)ptr);
		case SC_CARDCTL_CAC_INIT_GET_CERT_OBJECTS:
			return cac_get_init_and_get_count(&priv->pki_list, &priv->pki_current, (int *)ptr);
		case SC_CARDCTL_CAC_GET_NEXT_GENERIC_OBJECT:
			return cac_fill_object_info(&priv->general_list, &priv->general_current, (sc_pkcs15_data_info_t *)ptr);
		case SC_CARDCTL_CAC_GET_NEXT_CERT_OBJECT:
			return cac_fill_object_info(&priv->pki_list, &priv->pki_current, (sc_pkcs15_data_info_t *)ptr);
		case SC_CARDCTL_CAC_FINAL_GET_GENERIC_OBJECTS:
			return cac_final_iterator(&priv->general_list);
		case SC_CARDCTL_CAC_FINAL_GET_CERT_OBJECTS:
			return cac_final_iterator(&priv->pki_list);
	}

	LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
}