ves_icall_System_ValueType_Equals (MonoObject *this, MonoObject *that, MonoArray **fields)
{
	MonoClass *klass;
	MonoObject **values = NULL;
	MonoObject *o;
	MonoClassField* field;
	gpointer iter;
	int count = 0;

	MONO_ARCH_SAVE_REGS;

	MONO_CHECK_ARG_NULL (that);

	if (this->vtable != that->vtable)
		return FALSE;

	klass = mono_object_class (this);

	if (klass->enumtype && mono_class_enum_basetype (klass) && mono_class_enum_basetype (klass)->type == MONO_TYPE_I4)
		return (*(gint32*)((guint8*)this + sizeof (MonoObject)) == *(gint32*)((guint8*)that + sizeof (MonoObject)));

	/*
	 * Do the comparison for fields of primitive type and return a result if
	 * possible. Otherwise, return the remaining fields in an array to the 
	 * managed side. This way, we can avoid costly reflection operations in 
	 * managed code.
	 */
	*fields = NULL;
	iter = NULL;
	while ((field = mono_class_get_fields (klass, &iter))) {
		if (field->type->attrs & FIELD_ATTRIBUTE_STATIC)
			continue;
		if (mono_field_is_deleted (field))
			continue;
		/* FIXME: Add more types */
		switch (field->type->type) {
		case MONO_TYPE_U1:
		case MONO_TYPE_I1:
		case MONO_TYPE_BOOLEAN:
			if (*((guint8*)this + field->offset) != *((guint8*)that + field->offset))
				return FALSE;
			break;
		case MONO_TYPE_U2:
		case MONO_TYPE_I2:
		case MONO_TYPE_CHAR:
			if (*(gint16*)((guint8*)this + field->offset) != *(gint16*)((guint8*)that + field->offset))
				return FALSE;
			break;
		case MONO_TYPE_U4:
		case MONO_TYPE_I4:
			if (*(gint32*)((guint8*)this + field->offset) != *(gint32*)((guint8*)that + field->offset))
				return FALSE;
			break;
		case MONO_TYPE_U8:
		case MONO_TYPE_I8:
			if (*(gint64*)((guint8*)this + field->offset) != *(gint64*)((guint8*)that + field->offset))
				return FALSE;
			break;
		case MONO_TYPE_R4:
			if (*(float*)((guint8*)this + field->offset) != *(float*)((guint8*)that + field->offset))
				return FALSE;
			break;
		case MONO_TYPE_R8:
			if (*(double*)((guint8*)this + field->offset) != *(double*)((guint8*)that + field->offset))
				return FALSE;
			break;


		case MONO_TYPE_STRING: {
			MonoString *s1, *s2;
			guint32 s1len, s2len;
			s1 = *(MonoString**)((guint8*)this + field->offset);
			s2 = *(MonoString**)((guint8*)that + field->offset);
			if (s1 == s2)
				break;
			if ((s1 == NULL) || (s2 == NULL))
				return FALSE;
			s1len = mono_string_length (s1);
			s2len = mono_string_length (s2);
			if (s1len != s2len)
				return FALSE;

			if (memcmp (mono_string_chars (s1), mono_string_chars (s2), s1len * sizeof (gunichar2)) != 0)
				return FALSE;
			break;
		}
		default:
			if (!values)
				values = g_newa (MonoObject*, mono_class_num_fields (klass) * 2);
			o = mono_field_get_value_object (mono_object_domain (this), field, this);
			values [count++] = o;
			o = mono_field_get_value_object (mono_object_domain (this), field, that);
			values [count++] = o;
		}

		if (klass->enumtype)
			/* enums only have one non-static field */
			break;
	}

	if (values) {
		int i;
		mono_gc_wbarrier_generic_store (fields, (MonoObject*) mono_array_new (mono_domain_get (), mono_defaults.object_class, count));
		for (i = 0; i < count; ++i)
			mono_array_setref (*fields, i, values [i]);
		return FALSE;
	} else {
		return TRUE;
	}
}