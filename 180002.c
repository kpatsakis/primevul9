ves_icall_type_is_assignable_from (MonoReflectionType *type, MonoReflectionType *c)
{
	MonoDomain *domain; 
	MonoClass *klass;
	MonoClass *klassc;

	MONO_ARCH_SAVE_REGS;

	g_assert (type != NULL);
	
	domain = ((MonoObject *)type)->vtable->domain;

	klass = mono_class_from_mono_type (type->type);
	klassc = mono_class_from_mono_type (c->type);

	if (type->type->byref && !c->type->byref)
		return FALSE;

	return mono_class_is_assignable_from (klass, klassc);
}