ssize_t sdma_get_cpu_to_sde_map(struct sdma_engine *sde, char *buf)
{
	mutex_lock(&process_to_sde_mutex);
	if (cpumask_empty(&sde->cpu_mask))
		snprintf(buf, PAGE_SIZE, "%s\n", "empty");
	else
		cpumap_print_to_pagebuf(true, buf, &sde->cpu_mask);
	mutex_unlock(&process_to_sde_mutex);
	return strnlen(buf, PAGE_SIZE);
}