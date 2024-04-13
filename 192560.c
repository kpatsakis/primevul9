coolkey_get_attribute_data_fixed(CK_ATTRIBUTE_TYPE attr_type, unsigned long fixed_attributes,
																sc_cardctl_coolkey_attribute_t *attr_out) {
	unsigned long cka_id = fixed_attributes & 0xf;
	unsigned long cka_class = ((fixed_attributes) >> 4) & 0x7;
	unsigned long mask, bit;

	if (attr_type == CKA_ID) {
		attr_out->attribute_length = 1;
		attr_out->attribute_value= &coolkey_static_cka_id[cka_id];
		return SC_SUCCESS;
	}
	if (attr_type == CKA_CLASS) {
		attr_out->attribute_data_type = SC_CARDCTL_COOLKEY_ATTR_TYPE_ULONG;
		attr_out->attribute_length = 4;
		attr_out->attribute_value = coolkey_static_cka_class[cka_class].class_value;
		return SC_SUCCESS;
	}
	/* If it matched, it must be one of the booleans */
	mask = coolkey_static_cka_class[cka_class].boolean_mask;
	bit = coolkey_get_fixed_boolean_bit(attr_type);
	/* attribute isn't in the list */
	if ((bit & mask) == 0) {
		return SC_ERROR_DATA_OBJECT_NOT_FOUND;
	}
	attr_out->attribute_length = 1;
	attr_out->attribute_value = bit & fixed_attributes ? &coolkey_static_true : &coolkey_static_false;
	return SC_SUCCESS;
}