mono_ArgIterator_IntGetNextArgType (MonoArgIterator *iter)
{
	gint i;
	MONO_ARCH_SAVE_REGS;
	
	i = iter->sig->sentinelpos + iter->next_arg;

	g_assert (i < iter->sig->param_count);

	return iter->sig->params [i];
}