ves_icall_System_Array_GetValue (MonoObject *this, MonoObject *idxs)
{
	MonoClass *ac, *ic;
	MonoArray *ao, *io;
	gint32 i, pos, *ind;

	MONO_ARCH_SAVE_REGS;

	MONO_CHECK_ARG_NULL (idxs);

	io = (MonoArray *)idxs;
	ic = (MonoClass *)io->obj.vtable->klass;
	
	ao = (MonoArray *)this;
	ac = (MonoClass *)ao->obj.vtable->klass;

	g_assert (ic->rank == 1);
	if (io->bounds != NULL || io->max_length !=  ac->rank)
		mono_raise_exception (mono_get_exception_argument (NULL, NULL));

	ind = (gint32 *)io->vector;

	if (ao->bounds == NULL) {
		if (*ind < 0 || *ind >= ao->max_length)
			mono_raise_exception (mono_get_exception_index_out_of_range ());

		return ves_icall_System_Array_GetValueImpl (this, *ind);
	}
	
	for (i = 0; i < ac->rank; i++)
		if ((ind [i] < ao->bounds [i].lower_bound) ||
		    (ind [i] >=  (mono_array_lower_bound_t)ao->bounds [i].length + ao->bounds [i].lower_bound))
			mono_raise_exception (mono_get_exception_index_out_of_range ());

	pos = ind [0] - ao->bounds [0].lower_bound;
	for (i = 1; i < ac->rank; i++)
		pos = pos*ao->bounds [i].length + ind [i] - 
			ao->bounds [i].lower_bound;

	return ves_icall_System_Array_GetValueImpl (this, pos);
}