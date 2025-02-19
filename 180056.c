ves_icall_System_ValueType_InternalGetHashCode (MonoObject *this, MonoArray **fields)
{
	MonoClass *klass;
	MonoObject **values = NULL;
	MonoObject *o;
	int count = 0;
	gint32 result = 0;
	MonoClassField* field;
	gpointer iter;

	MONO_ARCH_SAVE_REGS;

	klass = mono_object_class (this);

	if (mono_class_num_fields (klass) == 0)
		return mono_object_hash (this);

	/*
	 * Compute the starting value of the hashcode for fields of primitive
	 * types, and return the remaining fields in an array to the managed side.
	 * This way, we can avoid costly reflection operations in managed code.
	 */
	iter = NULL;
	while ((field = mono_class_get_fields (klass, &iter))) {
		if (field->type->attrs & FIELD_ATTRIBUTE_STATIC)
			continue;
		if (mono_field_is_deleted (field))
			continue;
		/* FIXME: Add more types */
		switch (field->type->type) {
		case MONO_TYPE_I4:
			result ^= *(gint32*)((guint8*)this + field->offset);
			break;
		case MONO_TYPE_STRING: {
			MonoString *s;
			s = *(MonoString**)((guint8*)this + field->offset);
			if (s != NULL)
				result ^= mono_string_hash (s);
			break;
		}
		default:
			if (!values)
				values = g_newa (MonoObject*, mono_class_num_fields (klass));
			o = mono_field_get_value_object (mono_object_domain (this), field, this);
			values [count++] = o;
		}
	}

	if (values) {
		int i;
		mono_gc_wbarrier_generic_store (fields, (MonoObject*) mono_array_new (mono_domain_get (), mono_defaults.object_class, count));
		for (i = 0; i < count; ++i)
			mono_array_setref (*fields, i, values [i]);
	} else {
		*fields = NULL;
	}
	return result;
}