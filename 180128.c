ves_icall_Type_GetGenericTypeDefinition_impl (MonoReflectionType *type)
{
	MonoClass *klass;
	MONO_ARCH_SAVE_REGS;

	if (type->type->byref)
		return NULL;

	klass = mono_class_from_mono_type (type->type);
	if (klass->generic_container) {
		return type; /* check this one */
	}
	if (klass->generic_class) {
		MonoClass *generic_class = klass->generic_class->container_class;

		if (generic_class->wastypebuilder && generic_class->reflection_info)
			return generic_class->reflection_info;
		else
			return mono_type_get_object (mono_object_domain (type), &generic_class->byval_arg);
	}
	return NULL;
}