static void __init kvm_smp_prepare_boot_cpu(void)
{
	kvm_guest_cpu_init();
	native_smp_prepare_boot_cpu();
	kvm_spinlock_init();
}