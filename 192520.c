coolkey_find_object(sc_card_t *card, sc_cardctl_coolkey_find_object_t *fobj)
{
	sc_cardctl_coolkey_object_t *obj = NULL;
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	int r;

	switch (fobj->type) {
	case SC_CARDCTL_COOLKEY_FIND_BY_ID:
		obj = coolkey_find_object_by_id(&priv->objects_list, fobj->find_id);
		break;
	case SC_CARDCTL_COOLKEY_FIND_BY_TEMPLATE:
		obj = coolkey_find_object_by_template(card, fobj->coolkey_template, fobj->template_count);
		break;
	default:
		break;
	}
	if (obj == NULL) {
		return SC_ERROR_DATA_OBJECT_NOT_FOUND;
	}
	if (obj->data == NULL) {
		r = coolkey_fill_object(card, obj);
		if (r < 0) {
			return r;
		}
	}
	fobj->obj = obj;
	return SC_SUCCESS;
}