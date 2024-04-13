e_binding_transform_enum_value_to_nick (GBinding *binding,
                                        const GValue *source_value,
                                        GValue *target_value,
                                        gpointer not_used)
{
	GEnumClass *enum_class;
	GEnumValue *enum_value;
	gint value;
	gboolean success = FALSE;

	g_return_val_if_fail (G_IS_BINDING (binding), FALSE);

	enum_class = g_type_class_peek (G_VALUE_TYPE (source_value));
	g_return_val_if_fail (G_IS_ENUM_CLASS (enum_class), FALSE);

	value = g_value_get_enum (source_value);
	enum_value = g_enum_get_value (enum_class, value);
	if (enum_value != NULL) {
		g_value_set_string (target_value, enum_value->value_nick);
		success = TRUE;
	}

	return success;
}