static void __init kvm_apf_trap_init(void)
{
	set_intr_gate(14, async_page_fault);
}