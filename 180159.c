ves_icall_System_Array_SetGenericValueImpl (MonoObject *this, guint32 pos, gpointer value)
{
	MonoClass *ac, *ec;
	MonoArray *ao;
	gint32 esize;
	gpointer *ea;

	MONO_ARCH_SAVE_REGS;

	ao = (MonoArray *)this;
	ac = (MonoClass *)ao->obj.vtable->klass;
	ec = ac->element_class;

	esize = mono_array_element_size (ac);
	ea = (gpointer*)((char*)ao->vector + (pos * esize));

	if (MONO_TYPE_IS_REFERENCE (&ec->byval_arg)) {
		g_assert (esize == sizeof (gpointer));
		mono_gc_wbarrier_generic_store (ea, *(gpointer*)value);
	} else {
		g_assert (ec->inited);
		if (ec->has_references)
			mono_gc_wbarrier_value_copy (ea, value, 1, ec);
		memcpy (ea, value, esize);
	}
}