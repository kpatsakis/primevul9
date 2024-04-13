static void action_result(unsigned long pfn, enum mf_action_page_type type,
			  enum mf_result result)
{
	trace_memory_failure_event(pfn, type, result);

	pr_err("Memory failure: %#lx: recovery action for %s: %s\n",
		pfn, action_page_types[type], action_name[result]);
}