ves_icall_System_Object_MemberwiseClone (MonoObject *this)
{
	MONO_ARCH_SAVE_REGS;

	return mono_object_clone (this);
}