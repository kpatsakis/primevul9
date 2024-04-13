ves_icall_MonoType_get_Assembly (MonoReflectionType *type)
{
	MonoDomain *domain = mono_domain_get (); 
	MonoClass *class = mono_class_from_mono_type (type->type);

	MONO_ARCH_SAVE_REGS;

	return mono_assembly_get_object (domain, class->image->assembly);
}