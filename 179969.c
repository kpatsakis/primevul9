ves_icall_System_Array_GetLongLength (MonoArray *this, gint32 dimension)
{
	gint32 rank = ((MonoObject *)this)->vtable->klass->rank;

	MONO_ARCH_SAVE_REGS;

	if ((dimension < 0) || (dimension >= rank))
		mono_raise_exception (mono_get_exception_index_out_of_range ());
	
	if (this->bounds == NULL)
 		return this->max_length;
 	
 	return this->bounds [dimension].length;
}