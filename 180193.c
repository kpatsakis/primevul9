ves_icall_System_Reflection_FieldInfo_GetUnmanagedMarshal (MonoReflectionField *field)
{
	MonoClass *klass = field->field->parent;
	MonoMarshalType *info;
	int i;

	if (klass->generic_container ||
	    (klass->generic_class && klass->generic_class->context.class_inst->is_open))
		return NULL;

	info = mono_marshal_load_type_info (klass);

	for (i = 0; i < info->num_fields; ++i) {
		if (info->fields [i].field == field->field) {
			if (!info->fields [i].mspec)
				return NULL;
			else
				return mono_reflection_marshal_from_marshal_spec (field->object.vtable->domain, klass, info->fields [i].mspec);
		}
	}

	return NULL;
}