ves_icall_MonoField_GetValueInternal (MonoReflectionField *field, MonoObject *obj)
{	
	MonoObject *o;
	MonoClassField *cf = field->field;
	MonoClass *klass;
	MonoVTable *vtable;
	MonoType *t;
	MonoDomain *domain = mono_object_domain (field); 
	gchar *v;
	gboolean is_static = FALSE;
	gboolean is_ref = FALSE;

	MONO_ARCH_SAVE_REGS;

	if (field->klass->image->assembly->ref_only)
		mono_raise_exception (mono_get_exception_invalid_operation (
					"It is illegal to get the value on a field on a type loaded using the ReflectionOnly methods."));
	
	if (mono_security_get_mode () == MONO_SECURITY_MODE_CORE_CLR)
		mono_security_core_clr_ensure_reflection_access_field (cf);

	mono_class_init (field->klass);

	if (cf->type->attrs & FIELD_ATTRIBUTE_STATIC)
		is_static = TRUE;

	if (obj && !is_static) {
		/* Check that the field belongs to the object */
		gboolean found = FALSE;
		MonoClass *k;

		for (k = obj->vtable->klass; k; k = k->parent) {
			if (k == cf->parent) {
				found = TRUE;
				break;
			}
		}

		if (!found) {
			char *msg = g_strdup_printf ("Field '%s' defined on type '%s' is not a field on the target object which is of type '%s'.", mono_field_get_name (cf), cf->parent->name, obj->vtable->klass->name);
			MonoException *ex = mono_get_exception_argument (NULL, msg);
			g_free (msg);
			mono_raise_exception (ex);
		}
	}

	t = mono_type_get_underlying_type (cf->type);
	switch (t->type) {
	case MONO_TYPE_STRING:
	case MONO_TYPE_OBJECT:
	case MONO_TYPE_CLASS:
	case MONO_TYPE_ARRAY:
	case MONO_TYPE_SZARRAY:
		is_ref = TRUE;
		break;
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
	case MONO_TYPE_R8:
	case MONO_TYPE_VALUETYPE:
		is_ref = t->byref;
		break;
	case MONO_TYPE_GENERICINST:
		if (mono_type_generic_inst_is_valuetype (t)) {
			is_ref = t->byref;
		} else {
			is_ref = TRUE;
		}
		break;
	default:
		g_error ("type 0x%x not handled in "
			 "ves_icall_Monofield_GetValue", t->type);
		return NULL;
	}

	vtable = NULL;
	if (is_static) {
		vtable = mono_class_vtable_full (domain, cf->parent, TRUE);
		if (!vtable->initialized && !(cf->type->attrs & FIELD_ATTRIBUTE_LITERAL))
			mono_runtime_class_init (vtable);
	}
	
	if (is_ref) {
		if (is_static) {
			mono_field_static_get_value (vtable, cf, &o);
		} else {
			mono_field_get_value (obj, cf, &o);
		}
		return o;
	}

	if (mono_class_is_nullable (mono_class_from_mono_type (cf->type))) {
		MonoClass *nklass = mono_class_from_mono_type (cf->type);
		guint8 *buf;

		/* Convert the Nullable structure into a boxed vtype */
		if (is_static)
			buf = (guint8*)vtable->data + cf->offset;
		else
			buf = (guint8*)obj + cf->offset;

		return mono_nullable_box (buf, nklass);
	}

	/* boxed value type */
	klass = mono_class_from_mono_type (cf->type);
	o = mono_object_new (domain, klass);
	v = ((gchar *) o) + sizeof (MonoObject);
	if (is_static) {
		mono_field_static_get_value (vtable, cf, v);
	} else {
		mono_field_get_value (obj, cf, v);
	}

	return o;
}