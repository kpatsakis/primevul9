e_binding_transform_enum_nick_to_value (GBinding *binding,
                                        const GValue *source_value,
                                        GValue *target_value,
                                        gpointer not_used)
{
	GEnumClass *enum_class;
	GEnumValue *enum_value;
	const gchar *string;
	gboolean success = FALSE;

	g_return_val_if_fail (G_IS_BINDING (binding), FALSE);

	enum_class = g_type_class_peek (G_VALUE_TYPE (target_value));
	g_return_val_if_fail (G_IS_ENUM_CLASS (enum_class), FALSE);

	string = g_value_get_string (source_value);
	enum_value = g_enum_get_value_by_nick (enum_class, string);
	if (enum_value != NULL) {
		g_value_set_enum (target_value, enum_value->value);
		success = TRUE;
	}

	return success;
}