void __init fork_init(void)
{
	int i;
#ifndef CONFIG_ARCH_TASK_STRUCT_ALLOCATOR
#ifndef ARCH_MIN_TASKALIGN
#define ARCH_MIN_TASKALIGN	0
#endif
	int align = max_t(int, L1_CACHE_BYTES, ARCH_MIN_TASKALIGN);

	/* create a slab on which task_structs can be allocated */
	task_struct_cachep = kmem_cache_create("task_struct",
			arch_task_struct_size, align,
			SLAB_PANIC|SLAB_NOTRACK|SLAB_ACCOUNT, NULL);
#endif

	/* do the arch specific task caches init */
	arch_task_cache_init();

	set_max_threads(MAX_THREADS);

	init_task.signal->rlim[RLIMIT_NPROC].rlim_cur = max_threads/2;
	init_task.signal->rlim[RLIMIT_NPROC].rlim_max = max_threads/2;
	init_task.signal->rlim[RLIMIT_SIGPENDING] =
		init_task.signal->rlim[RLIMIT_NPROC];

	for (i = 0; i < UCOUNT_COUNTS; i++) {
		init_user_ns.ucount_max[i] = max_threads/2;
	}

#ifdef CONFIG_VMAP_STACK
	cpuhp_setup_state(CPUHP_BP_PREPARE_DYN, "fork:vm_stack_cache",
			  NULL, free_vm_stack_cache);
#endif

	lockdep_init_task(&init_task);
}