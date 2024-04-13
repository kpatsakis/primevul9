ves_icall_MonoMethod_get_name (MonoReflectionMethod *m)
{
	MonoMethod *method = m->method;

	MONO_OBJECT_SETREF (m, name, mono_string_new (mono_object_domain (m), method->name));
	return m->name;
}