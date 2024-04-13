ves_icall_System_Array_GetLowerBound (MonoArray *this, gint32 dimension)
{
	gint32 rank = ((MonoObject *)this)->vtable->klass->rank;

	MONO_ARCH_SAVE_REGS;

	if ((dimension < 0) || (dimension >= rank))
		mono_raise_exception (mono_get_exception_index_out_of_range ());
	
	if (this->bounds == NULL)
		return 0;
	
	return this->bounds [dimension].lower_bound;
}