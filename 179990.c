ves_icall_MonoType_GetElementType (MonoReflectionType *type)
{
	MonoClass *class;

	MONO_ARCH_SAVE_REGS;

	if (!type->type->byref && type->type->type == MONO_TYPE_SZARRAY)
		return mono_type_get_object (mono_object_domain (type), &type->type->data.klass->byval_arg);

	class = mono_class_from_mono_type (type->type);

	// GetElementType should only return a type for:
	// Array Pointer PassedByRef
	if (type->type->byref)
		return mono_type_get_object (mono_object_domain (type), &class->byval_arg);
	else if (class->element_class && MONO_CLASS_IS_ARRAY (class))
		return mono_type_get_object (mono_object_domain (type), &class->element_class->byval_arg);
	else if (class->element_class && type->type->type == MONO_TYPE_PTR)
		return mono_type_get_object (mono_object_domain (type), &class->element_class->byval_arg);
	else
		return NULL;
}