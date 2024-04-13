ves_icall_Type_GetNestedType (MonoReflectionType *type, MonoString *name, guint32 bflags)
{
	MonoDomain *domain; 
	MonoClass *klass;
	MonoClass *nested;
	char *str;
	gpointer iter;
	
	MONO_ARCH_SAVE_REGS;

	if (name == NULL)
		mono_raise_exception (mono_get_exception_argument_null ("name"));
	
	domain = ((MonoObject *)type)->vtable->domain;
	if (type->type->byref)
		return NULL;
	klass = mono_class_from_mono_type (type->type);
	str = mono_string_to_utf8 (name);

 handle_parent:
	if (klass->exception_type != MONO_EXCEPTION_NONE)
		mono_raise_exception (mono_class_get_exception_for_failure (klass));

	/*
	 * If a nested type is generic, return its generic type definition.
	 * Note that this means that the return value is essentially a
	 * nested type of the generic type definition of @klass.
	 *
	 * A note in MSDN claims that a generic type definition can have
	 * nested types that aren't generic.  In any case, the container of that
	 * nested type would be the generic type definition.
	 */
	if (klass->generic_class)
		klass = klass->generic_class->container_class;

	iter = NULL;
	while ((nested = mono_class_get_nested_types (klass, &iter))) {
		int match = 0;
		if ((nested->flags & TYPE_ATTRIBUTE_VISIBILITY_MASK) == TYPE_ATTRIBUTE_NESTED_PUBLIC) {
			if (bflags & BFLAGS_Public)
				match++;
		} else {
			if (bflags & BFLAGS_NonPublic)
				match++;
		}
		if (!match)
			continue;
		if (strcmp (nested->name, str) == 0){
			g_free (str);
			return mono_type_get_object (domain, &nested->byval_arg);
		}
	}
	if (!(bflags & BFLAGS_DeclaredOnly) && (klass = klass->parent))
		goto handle_parent;
	g_free (str);
	return NULL;
}