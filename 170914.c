rb_handle_timestamp(struct ring_buffer_per_cpu *cpu_buffer,
		    struct rb_event_info *info)
{
	WARN_ONCE(info->delta > (1ULL << 59),
		  KERN_WARNING "Delta way too big! %llu ts=%llu write stamp = %llu\n%s",
		  (unsigned long long)info->delta,
		  (unsigned long long)info->ts,
		  (unsigned long long)cpu_buffer->write_stamp,
		  sched_clock_stable() ? "" :
		  "If you just came from a suspend/resume,\n"
		  "please switch to the trace global clock:\n"
		  "  echo global > /sys/kernel/debug/tracing/trace_clock\n");
	info->add_timestamp = 1;
}