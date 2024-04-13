ves_icall_MonoField_GetRawConstantValue (MonoReflectionField *this)
{	
	MonoObject *o = NULL;
	MonoClassField *field = this->field;
	MonoClass *klass;
	MonoDomain *domain = mono_object_domain (this); 
	gchar *v;
	MonoTypeEnum def_type;
	const char *def_value;

	MONO_ARCH_SAVE_REGS;
	
	mono_class_init (field->parent);

	if (!(field->type->attrs & FIELD_ATTRIBUTE_HAS_DEFAULT))
		mono_raise_exception (mono_get_exception_invalid_operation (NULL));

	if (field->parent->image->dynamic) {
		/* FIXME: */
		g_assert_not_reached ();
	}

	def_value = mono_class_get_field_default_value (field, &def_type);

	switch (def_type) {
	case MONO_TYPE_U1:
	case MONO_TYPE_I1:
	case MONO_TYPE_BOOLEAN:
	case MONO_TYPE_U2:
	case MONO_TYPE_I2:
	case MONO_TYPE_CHAR:
	case MONO_TYPE_U:
	case MONO_TYPE_I:
	case MONO_TYPE_U4:
	case MONO_TYPE_I4:
	case MONO_TYPE_R4:
	case MONO_TYPE_U8:
	case MONO_TYPE_I8:
	case MONO_TYPE_R8: {
		MonoType *t;

		/* boxed value type */
		t = g_new0 (MonoType, 1);
		t->type = def_type;
		klass = mono_class_from_mono_type (t);
		g_free (t);
		o = mono_object_new (domain, klass);
		v = ((gchar *) o) + sizeof (MonoObject);
		mono_get_constant_value_from_blob (domain, def_type, def_value, v);
		break;
	}
	case MONO_TYPE_STRING:
	case MONO_TYPE_CLASS:
		mono_get_constant_value_from_blob (domain, def_type, def_value, &o);
		break;
	default:
		g_assert_not_reached ();
	}

	return o;
}