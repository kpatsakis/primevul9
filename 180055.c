mono_ArgIterator_IntGetNextArg (MonoArgIterator *iter)
{
	guint32 i, arg_size;
	gint32 align;
	MonoTypedRef res;
	MONO_ARCH_SAVE_REGS;

	i = iter->sig->sentinelpos + iter->next_arg;

	g_assert (i < iter->sig->param_count);

	res.type = iter->sig->params [i];
	res.klass = mono_class_from_mono_type (res.type);
	res.value = iter->args;
	arg_size = mono_type_stack_size (res.type, &align);
#if G_BYTE_ORDER != G_LITTLE_ENDIAN
	if (arg_size <= sizeof (gpointer)) {
		int dummy;
		int padding = arg_size - mono_type_size (res.type, &dummy);
		res.value = (guint8*)res.value + padding;
	}
#endif
	iter->args = (char*)iter->args + arg_size;
	iter->next_arg++;

	/* g_print ("returning arg %d, type 0x%02x of size %d at %p\n", i, res.type->type, arg_size, res.value); */

	return res;
}