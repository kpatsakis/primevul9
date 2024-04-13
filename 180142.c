ves_icall_System_Enum_get_underlying_type (MonoReflectionType *type)
{
	MonoType *etype;

	MONO_ARCH_SAVE_REGS;

	etype = mono_class_enum_basetype (mono_class_from_mono_type (type->type));
	if (!etype)
		/* MS throws this for typebuilders */
		mono_raise_exception (mono_get_exception_argument ("Type must be a type provided by the runtime.", "enumType"));

	return mono_type_get_object (mono_object_domain (type), etype);
}