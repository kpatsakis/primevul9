static inline bool boot_cpu_is_amd(void)
{
	WARN_ON_ONCE(!tdp_enabled);
	return shadow_x_mask == 0;
}