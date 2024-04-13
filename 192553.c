coolkey_find_object_by_template(sc_card_t *card, sc_cardctl_coolkey_attribute_t *template, int count)
{
	list_t *list;
	sc_cardctl_coolkey_object_t *current, *rv = NULL;
	coolkey_private_data_t * priv = COOLKEY_DATA(card);
	int i, r;
	unsigned int tmp_pos = (unsigned int) -1;

	list = &priv->objects_list;
	if (list->iter_active) {
		/* workaround missing functionality of second iterator */
		tmp_pos = list->iter_pos;
		list_iterator_stop(list);
	}

	list_iterator_start(list);
	while (list_iterator_hasnext(list)) {
		sc_cardctl_coolkey_attribute_t attribute;
		current = list_iterator_next(list);
		attribute.object = current;

		for (i=0; i < count; i++) {
			attribute.attribute_type = template[i].attribute_type;
			r = coolkey_find_attribute(card, &attribute);
			if (r < 0) {
				break;
			}
			if (template[i].attribute_data_type != attribute.attribute_data_type) {
				break;
			}
			if (template[i].attribute_length != attribute.attribute_length) {
				break;
			}
			if (memcmp(attribute.attribute_value, template[i].attribute_value,
							attribute.attribute_length) != 0) {
				break;
			}
		}
		/* just return the first one */
		if (i == count) {
			rv = current;
			break;
		}
	}

	list_iterator_stop(list);
	if (tmp_pos != (unsigned int)-1) {
		/* workaround missing functionality of second iterator */
		list_iterator_start(list);
		while (list_iterator_hasnext(list) && list->iter_pos < tmp_pos)
			(void) list_iterator_next(list);
	}
	return rv;
}