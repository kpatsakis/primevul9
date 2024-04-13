static void kvm_guest_cpu_offline(void *dummy)
{
	kvm_disable_steal_time();
	if (kvm_para_has_feature(KVM_FEATURE_PV_EOI))
		wrmsrl(MSR_KVM_PV_EOI_EN, 0);
	kvm_pv_disable_apf();
	apf_task_wake_all();
}