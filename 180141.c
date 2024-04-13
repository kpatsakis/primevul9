ves_icall_type_iscomobject (MonoReflectionType *type)
{
	MonoClass *klass = mono_class_from_mono_type (type->type);
	MONO_ARCH_SAVE_REGS;

	return (klass && klass->is_com_object);
}