ves_icall_MonoField_SetValueInternal (MonoReflectionField *field, MonoObject *obj, MonoObject *value)
{
	MonoClassField *cf = field->field;
	gchar *v;

	MONO_ARCH_SAVE_REGS;

	if (field->klass->image->assembly->ref_only)
		mono_raise_exception (mono_get_exception_invalid_operation (
					"It is illegal to set the value on a field on a type loaded using the ReflectionOnly methods."));

	if (mono_security_get_mode () == MONO_SECURITY_MODE_CORE_CLR)
		mono_security_core_clr_ensure_reflection_access_field (cf);

	v = (gchar *) value;
	if (!cf->type->byref) {
		switch (cf->type->type) {
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
			if (v != NULL)
				v += sizeof (MonoObject);
			break;
		case MONO_TYPE_STRING:
		case MONO_TYPE_OBJECT:
		case MONO_TYPE_CLASS:
		case MONO_TYPE_ARRAY:
		case MONO_TYPE_SZARRAY:
			/* Do nothing */
			break;
		case MONO_TYPE_GENERICINST: {
			MonoGenericClass *gclass = cf->type->data.generic_class;
			g_assert (!gclass->context.class_inst->is_open);

			if (mono_class_is_nullable (mono_class_from_mono_type (cf->type))) {
				MonoClass *nklass = mono_class_from_mono_type (cf->type);
				MonoObject *nullable;

				/* 
				 * Convert the boxed vtype into a Nullable structure.
				 * This is complicated by the fact that Nullables have
				 * a variable structure.
				 */
				nullable = mono_object_new (mono_domain_get (), nklass);

				mono_nullable_init (mono_object_unbox (nullable), value, nklass);

				v = mono_object_unbox (nullable);
			}
			else 
				if (gclass->container_class->valuetype && (v != NULL))
					v += sizeof (MonoObject);
			break;
		}
		default:
			g_error ("type 0x%x not handled in "
				 "ves_icall_FieldInfo_SetValueInternal", cf->type->type);
			return;
		}
	}

	if (cf->type->attrs & FIELD_ATTRIBUTE_STATIC) {
		MonoVTable *vtable = mono_class_vtable_full (mono_object_domain (field), cf->parent, TRUE);
		if (!vtable->initialized)
			mono_runtime_class_init (vtable);
		mono_field_static_set_value (vtable, cf, v);
	} else {
		mono_field_set_value (obj, cf, v);
	}
}