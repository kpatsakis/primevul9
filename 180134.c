ves_icall_System_Array_GetLength (MonoArray *this, gint32 dimension)
{
	gint32 rank = ((MonoObject *)this)->vtable->klass->rank;
	mono_array_size_t length;

	MONO_ARCH_SAVE_REGS;

	if ((dimension < 0) || (dimension >= rank))
		mono_raise_exception (mono_get_exception_index_out_of_range ());
	
	if (this->bounds == NULL)
		length = this->max_length;
	else
		length = this->bounds [dimension].length;

#ifdef MONO_BIG_ARRAYS
	if (length > G_MAXINT32)
	        mono_raise_exception (mono_get_exception_overflow ());
#endif
	return length;
}