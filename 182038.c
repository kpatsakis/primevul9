static void kvm_kick_cpu(int cpu)
{
	int apicid;
	unsigned long flags = 0;

	apicid = per_cpu(x86_cpu_to_apicid, cpu);
	kvm_hypercall2(KVM_HC_KICK_CPU, flags, apicid);
}