ves_icall_MonoType_get_Namespace (MonoReflectionType *type)
{
	MonoDomain *domain = mono_domain_get (); 
	MonoClass *class = mono_class_from_mono_type (type->type);

	MONO_ARCH_SAVE_REGS;

	while (class->nested_in)
		class = class->nested_in;

	if (class->name_space [0] == '\0')
		return NULL;
	else
		return mono_string_new (domain, class->name_space);
}