ves_icall_System_Enum_get_value (MonoObject *this)
{
	MonoObject *res;
	MonoClass *enumc;
	gpointer dst;
	gpointer src;
	int size;

	MONO_ARCH_SAVE_REGS;

	if (!this)
		return NULL;

	g_assert (this->vtable->klass->enumtype);
	
	enumc = mono_class_from_mono_type (mono_class_enum_basetype (this->vtable->klass));
	res = mono_object_new (mono_object_domain (this), enumc);
	dst = (char *)res + sizeof (MonoObject);
	src = (char *)this + sizeof (MonoObject);
	size = mono_class_value_size (enumc, NULL);

	memcpy (dst, src, size);

	return res;
}