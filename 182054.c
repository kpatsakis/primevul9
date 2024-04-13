static void kvm_pv_disable_apf(void)
{
	if (!__this_cpu_read(apf_reason.enabled))
		return;

	wrmsrl(MSR_KVM_ASYNC_PF_EN, 0);
	__this_cpu_write(apf_reason.enabled, 0);

	printk(KERN_INFO"Unregister pv shared memory for cpu %d\n",
	       smp_processor_id());
}