ves_icall_IsTransparentProxy (MonoObject *proxy)
{
	MONO_ARCH_SAVE_REGS;

	if (!proxy)
		return 0;

	if (proxy->vtable->klass == mono_defaults.transparent_proxy_class)
		return 1;

	return 0;
}