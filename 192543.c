coolkey_list_object(sc_card_t *card, u8 seq, coolkey_object_info_t *object_info)
{
	u8 *rbuf = (u8 *) object_info;
	size_t rbuflen = sizeof(*object_info);

	return coolkey_apdu_io(card, COOLKEY_CLASS, COOLKEY_INS_LIST_OBJECTS, seq, 0,
			NULL, 0, &rbuf, &rbuflen, NULL, 0);

}