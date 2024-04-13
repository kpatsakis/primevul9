static int load_vdso32(void)
{
	int ret;

	if (vdso32_enabled != 1)  /* Other values all mean "disabled" */
		return 0;

	ret = map_vdso(selected_vdso32, false);
	if (ret)
		return ret;

	if (selected_vdso32->sym_VDSO32_SYSENTER_RETURN)
		current_thread_info()->sysenter_return =
			current->mm->context.vdso +
			selected_vdso32->sym_VDSO32_SYSENTER_RETURN;

	return 0;
}