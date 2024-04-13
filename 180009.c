check_for_invalid_type (MonoClass *klass)
{
	char *name;
	MonoString *str;
	if (klass->byval_arg.type != MONO_TYPE_TYPEDBYREF)
		return;

	name = mono_type_get_full_name (klass);
	str =  mono_string_new (mono_domain_get (), name);
	g_free (name);
	mono_raise_exception ((MonoException*)mono_get_exception_type_load (str, NULL));

}