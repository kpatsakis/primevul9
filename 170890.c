ring_buffer_free(struct ring_buffer *buffer)
{
	int cpu;

#ifdef CONFIG_HOTPLUG_CPU
	cpu_notifier_register_begin();
	__unregister_cpu_notifier(&buffer->cpu_notify);
#endif

	for_each_buffer_cpu(buffer, cpu)
		rb_free_cpu_buffer(buffer->buffers[cpu]);

#ifdef CONFIG_HOTPLUG_CPU
	cpu_notifier_register_done();
#endif

	kfree(buffer->buffers);
	free_cpumask_var(buffer->cpumask);

	kfree(buffer);
}