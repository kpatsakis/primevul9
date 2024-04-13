ves_icall_System_Activator_CreateInstanceInternal (MonoReflectionType *type)
{
	MonoClass *klass;
	MonoDomain *domain;
	
	MONO_ARCH_SAVE_REGS;

	domain = mono_object_domain (type);
	klass = mono_class_from_mono_type (type->type);

	if (mono_class_is_nullable (klass))
		/* No arguments -> null */
		return NULL;

	return mono_object_new (domain, klass);
}