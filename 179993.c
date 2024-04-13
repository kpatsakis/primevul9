ves_icall_Type_GetConstructors_internal (MonoReflectionType *type, guint32 bflags, MonoReflectionType *reftype)
{
	MonoDomain *domain; 
	static MonoClass *System_Reflection_ConstructorInfo;
	MonoClass *startklass, *klass, *refklass;
	MonoArray *res;
	MonoMethod *method;
	MonoObject *member;
	int i, match;
	gpointer iter = NULL;
	MonoPtrArray tmp_array;
	
	MONO_ARCH_SAVE_REGS;

	mono_ptr_array_init (tmp_array, 4); /*FIXME, guestimating*/

	domain = ((MonoObject *)type)->vtable->domain;
	if (type->type->byref)
		return mono_array_new_cached (domain, mono_defaults.method_info_class, 0);
	klass = startklass = mono_class_from_mono_type (type->type);
	refklass = mono_class_from_mono_type (reftype->type);

	if (klass->exception_type != MONO_EXCEPTION_NONE)
		mono_raise_exception (mono_class_get_exception_for_failure (klass));

	if (!System_Reflection_ConstructorInfo)
		System_Reflection_ConstructorInfo = mono_class_from_name (
			mono_defaults.corlib, "System.Reflection", "ConstructorInfo");

	iter = NULL;
	while ((method = mono_class_get_methods (klass, &iter))) {
		match = 0;
		if (strcmp (method->name, ".ctor") && strcmp (method->name, ".cctor"))
			continue;
		if ((method->flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC) {
			if (bflags & BFLAGS_Public)
				match++;
		} else {
			if (bflags & BFLAGS_NonPublic)
				match++;
		}
		if (!match)
			continue;
		match = 0;
		if (method->flags & METHOD_ATTRIBUTE_STATIC) {
			if (bflags & BFLAGS_Static)
				if ((bflags & BFLAGS_FlattenHierarchy) || (klass == startklass))
					match++;
		} else {
			if (bflags & BFLAGS_Instance)
				match++;
		}

		if (!match)
			continue;
		member = (MonoObject*)mono_method_get_object (domain, method, refklass);

		mono_ptr_array_append (tmp_array, member);
	}

	res = mono_array_new_cached (domain, System_Reflection_ConstructorInfo, mono_ptr_array_size (tmp_array));

	for (i = 0; i < mono_ptr_array_size (tmp_array); ++i)
		mono_array_setref (res, i, mono_ptr_array_get (tmp_array, i));

	mono_ptr_array_destroy (tmp_array);

	return res;
}