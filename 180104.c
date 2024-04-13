ves_icall_MonoType_get_UnderlyingSystemType (MonoReflectionType *type)
{
	MonoDomain *domain = mono_domain_get (); 
	MonoClass *class = mono_class_from_mono_type (type->type);

	MONO_ARCH_SAVE_REGS;

	if (class->enumtype && mono_class_enum_basetype (class)) /* types that are modified typebuilders may not have enum_basetype set */
		return mono_type_get_object (domain, mono_class_enum_basetype (class));
	else if (class->element_class)
		return mono_type_get_object (domain, &class->element_class->byval_arg);
	else
		return NULL;
}