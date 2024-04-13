static int cac_parse_cardurl(sc_card_t *card, cac_private_data_t *priv, cac_card_url_t *val, int len)
{
	cac_object_t new_object;
	const cac_object_t *obj;
	unsigned short object_id;
	int r;

	r = cac_path_from_cardurl(card, &new_object.path, val, len);
	if (r != SC_SUCCESS) {
		return r;
	}
	switch (val->cardApplicationType) {
	case CAC_APP_TYPE_PKI:
		/* we don't want to overflow the cac_label array. This test could
		 * go way if we create a label function that will create a unique label
		 * from a cert index.
		 */
		if (priv->cert_next >= MAX_CAC_SLOTS)
			break; /* don't fail just because we have more certs than we can support */
		new_object.name = cac_labels[priv->cert_next];
		new_object.fd = priv->cert_next+1;
		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"CARDURL: pki_object found, cert_next=%d (%s),", priv->cert_next, new_object.name);
		cac_add_object_to_list(&priv->pki_list, &new_object);
		priv->cert_next++;
		break;
	case CAC_APP_TYPE_GENERAL:
		object_id = bebytes2ushort(val->objectID);
		obj = cac_find_obj_by_id(object_id);
		if (obj == NULL)
			break; /* don't fail just because we don't recognize the object */
		new_object.name = obj->name;
		new_object.fd = 0;
		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"CARDURL: gen_object found, objectID=%x (%s),", object_id, new_object.name);
		cac_add_object_to_list(&priv->general_list, &new_object);
		break;
	case CAC_APP_TYPE_SKI:
		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"CARDURL: ski_object found");
	break;
	default:
		sc_debug(card->ctx, SC_LOG_DEBUG_VERBOSE,"CARDURL: unknown object_object found (type=0x%02x)", val->cardApplicationType);
		/* don't fail just because there is an unknown object in the CCC */
		break;
	}
	return SC_SUCCESS;
}