ves_icall_System_Array_GetRank (MonoObject *this)
{
	MONO_ARCH_SAVE_REGS;

	return this->vtable->klass->rank;
}