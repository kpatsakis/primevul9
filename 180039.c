mono_ArgIterator_Setup (MonoArgIterator *iter, char* argsp, char* start)
{
	MONO_ARCH_SAVE_REGS;

	iter->sig = *(MonoMethodSignature**)argsp;
	
	g_assert (iter->sig->sentinelpos <= iter->sig->param_count);
	g_assert (iter->sig->call_convention == MONO_CALL_VARARG);

	iter->next_arg = 0;
	/* FIXME: it's not documented what start is exactly... */
	if (start) {
		iter->args = start;
	} else {
		iter->args = argsp + sizeof (gpointer);
#ifndef MONO_ARCH_REGPARMS
		{
		guint32 i, arg_size;
		gint32 align;
		for (i = 0; i < iter->sig->sentinelpos; ++i) {
			arg_size = mono_type_stack_size (iter->sig->params [i], &align);
			iter->args = (char*)iter->args + arg_size;
		}
		}
#endif
	}
	iter->num_args = iter->sig->param_count - iter->sig->sentinelpos;

	/* g_print ("sig %p, param_count: %d, sent: %d\n", iter->sig, iter->sig->param_count, iter->sig->sentinelpos); */
}