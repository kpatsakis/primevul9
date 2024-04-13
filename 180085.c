ves_icall_System_Array_SetValue (MonoArray *this, MonoObject *value,
				 MonoArray *idxs)
{
	MonoClass *ac, *ic;
	gint32 i, pos, *ind;

	MONO_ARCH_SAVE_REGS;

	MONO_CHECK_ARG_NULL (idxs);

	ic = idxs->obj.vtable->klass;
	ac = this->obj.vtable->klass;

	g_assert (ic->rank == 1);
	if (idxs->bounds != NULL || idxs->max_length != ac->rank)
		mono_raise_exception (mono_get_exception_argument (NULL, NULL));

	ind = (gint32 *)idxs->vector;

	if (this->bounds == NULL) {
		if (*ind < 0 || *ind >= this->max_length)
			mono_raise_exception (mono_get_exception_index_out_of_range ());

		ves_icall_System_Array_SetValueImpl (this, value, *ind);
		return;
	}
	
	for (i = 0; i < ac->rank; i++)
		if ((ind [i] < this->bounds [i].lower_bound) ||
		    (ind [i] >= (mono_array_lower_bound_t)this->bounds [i].length + this->bounds [i].lower_bound))
			mono_raise_exception (mono_get_exception_index_out_of_range ());

	pos = ind [0] - this->bounds [0].lower_bound;
	for (i = 1; i < ac->rank; i++)
		pos = pos * this->bounds [i].length + ind [i] - 
			this->bounds [i].lower_bound;

	ves_icall_System_Array_SetValueImpl (this, value, pos);
}