u32 kvm_read_and_reset_pf_reason(void)
{
	u32 reason = 0;

	if (__this_cpu_read(apf_reason.enabled)) {
		reason = __this_cpu_read(apf_reason.reason);
		__this_cpu_write(apf_reason.reason, 0);
	}

	return reason;
}