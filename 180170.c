mono_TypedReference_ToObjectInternal (MonoType *type, gpointer value, MonoClass *klass)
{
	MONO_ARCH_SAVE_REGS;

	if (MONO_TYPE_IS_REFERENCE (type)) {
		MonoObject** objp = value;
		return *objp;
	}

	return mono_value_box (mono_domain_get (), klass, value);
}