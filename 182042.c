static void kvm_restore_sched_clock_state(void)
{
	kvm_register_clock("primary cpu clock, resume");
}