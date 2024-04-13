int arch_setup_additional_pages(struct linux_binprm *bprm, int uses_interp)
{
	return load_vdso32();
}