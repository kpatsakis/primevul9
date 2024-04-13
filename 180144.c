ves_icall_get_enum_info (MonoReflectionType *type, MonoEnumInfo *info)
{
	MonoDomain *domain = mono_object_domain (type); 
	MonoClass *enumc = mono_class_from_mono_type (type->type);
	guint j = 0, nvalues, crow;
	gpointer iter;
	MonoClassField *field;

	MONO_ARCH_SAVE_REGS;

	MONO_STRUCT_SETREF (info, utype, mono_type_get_object (domain, mono_class_enum_basetype (enumc)));
	nvalues = mono_class_num_fields (enumc) ? mono_class_num_fields (enumc) - 1 : 0;
	MONO_STRUCT_SETREF (info, names, mono_array_new (domain, mono_defaults.string_class, nvalues));
	MONO_STRUCT_SETREF (info, values, mono_array_new (domain, enumc, nvalues));

	crow = -1;
	iter = NULL;
	while ((field = mono_class_get_fields (enumc, &iter))) {
		const char *p;
		int len;
		MonoTypeEnum def_type;
		
		if (strcmp ("value__", mono_field_get_name (field)) == 0)
			continue;
		if (mono_field_is_deleted (field))
			continue;
		mono_array_setref (info->names, j, mono_string_new (domain, mono_field_get_name (field)));

		p = mono_class_get_field_default_value (field, &def_type);
		len = mono_metadata_decode_blob_size (p, &p);
		switch (mono_class_enum_basetype (enumc)->type) {
		case MONO_TYPE_U1:
		case MONO_TYPE_I1:
			mono_array_set (info->values, gchar, j, *p);
			break;
		case MONO_TYPE_CHAR:
		case MONO_TYPE_U2:
		case MONO_TYPE_I2:
			mono_array_set (info->values, gint16, j, read16 (p));
			break;
		case MONO_TYPE_U4:
		case MONO_TYPE_I4:
			mono_array_set (info->values, gint32, j, read32 (p));
			break;
		case MONO_TYPE_U8:
		case MONO_TYPE_I8:
			mono_array_set (info->values, gint64, j, read64 (p));
			break;
		default:
			g_error ("Implement type 0x%02x in get_enum_info", mono_class_enum_basetype (enumc)->type);
		}
		++j;
	}
}