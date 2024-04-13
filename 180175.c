ves_icall_MonoType_get_Name (MonoReflectionType *type)
{
	MonoDomain *domain = mono_domain_get (); 
	MonoClass *class = mono_class_from_mono_type (type->type);

	MONO_ARCH_SAVE_REGS;

	if (type->type->byref) {
		char *n = g_strdup_printf ("%s&", class->name);
		MonoString *res = mono_string_new (domain, n);

		g_free (n);

		return res;
	} else {
		return mono_string_new (domain, class->name);
	}
}