static int coolkey_card_ctl(sc_card_t *card, unsigned long cmd, void *ptr)
{
	coolkey_private_data_t * priv = COOLKEY_DATA(card);

	LOG_FUNC_CALLED(card->ctx);
	sc_log(card->ctx, "cmd=%ld ptr=%p", cmd, ptr);

	if (priv == NULL) {
		LOG_FUNC_RETURN(card->ctx, SC_ERROR_INTERNAL);
	}
	switch(cmd) {
		case SC_CARDCTL_GET_SERIALNR:
			return coolkey_get_serial_nr_from_CUID(card, (sc_serial_number_t *) ptr);
		case SC_CARDCTL_COOLKEY_GET_TOKEN_INFO:
			return coolkey_get_token_info(card, (sc_pkcs15_tokeninfo_t *) ptr);
		case SC_CARDCTL_COOLKEY_FIND_OBJECT:
			return coolkey_find_object(card, (sc_cardctl_coolkey_find_object_t *)ptr);
		case SC_CARDCTL_COOLKEY_INIT_GET_OBJECTS:
			return coolkey_get_init_and_get_count(&priv->objects_list, (int *)ptr);
		case SC_CARDCTL_COOLKEY_GET_NEXT_OBJECT:
			return coolkey_fetch_object(&priv->objects_list, (sc_cardctl_coolkey_object_t *)ptr);
		case SC_CARDCTL_COOLKEY_FINAL_GET_OBJECTS:
			return coolkey_final_iterator(&priv->objects_list);
		case SC_CARDCTL_COOLKEY_GET_ATTRIBUTE:
			return coolkey_find_attribute(card,(sc_cardctl_coolkey_attribute_t *)ptr);
	}

	LOG_FUNC_RETURN(card->ctx, SC_ERROR_NOT_SUPPORTED);
}