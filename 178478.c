static ssize_t remove_cpu_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t count)
{
	u64 cpu, lapicid;
	int ret;

	if (kstrtoull(buf, 0, &cpu) < 0)
		return -EINVAL;

	if (cpu >= num_possible_cpus() || cpu == 0 || !cpu_is_hotpluggable(cpu))
		return -EINVAL;

	if (cpu_online(cpu))
		remove_cpu(cpu);

	lapicid = cpu_data(cpu).apicid;
	dev_dbg(dev, "Try to remove cpu %lld with lapicid %lld\n", cpu, lapicid);
	ret = hcall_sos_remove_cpu(lapicid);
	if (ret < 0) {
		dev_err(dev, "Failed to remove cpu %lld!\n", cpu);
		goto fail_remove;
	}

	return count;

fail_remove:
	add_cpu(cpu);
	return ret;
}