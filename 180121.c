mono_TypedReference_ToObject (MonoTypedRef tref)
{
	MONO_ARCH_SAVE_REGS;

	if (MONO_TYPE_IS_REFERENCE (tref.type)) {
		MonoObject** objp = tref.value;
		return *objp;
	}

	return mono_value_box (mono_domain_get (), tref.klass, tref.value);
}