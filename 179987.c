ves_icall_Type_MakeGenericType (MonoReflectionType *type, MonoArray *type_array)
{
	MonoClass *class;
	MonoType *geninst, **types;
	int i, count;

	MONO_ARCH_SAVE_REGS;

	count = mono_array_length (type_array);
	types = g_new0 (MonoType *, count);

	for (i = 0; i < count; i++) {
		MonoReflectionType *t = mono_array_get (type_array, gpointer, i);
		types [i] = t->type;
	}

	geninst = mono_reflection_bind_generic_parameters (type, count, types);
	g_free (types);
	if (!geninst)
		return NULL;

	class = mono_class_from_mono_type (geninst);

	/*we might inflate to the GTD*/
	if (class->generic_class && !mono_verifier_class_is_valid_generic_instantiation (class))
		mono_raise_exception (mono_get_exception_argument ("method", "Invalid generic arguments"));

	return mono_type_get_object (mono_object_domain (type), geninst);
}