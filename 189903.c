int kvm_cpu_dirty_log_size(void)
{
	return kvm_x86_ops.cpu_dirty_log_size;
}