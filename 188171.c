static int __init oom_init(void)
{
	oom_reaper_th = kthread_run(oom_reaper, NULL, "oom_reaper");
	if (IS_ERR(oom_reaper_th)) {
		pr_err("Unable to start OOM reaper %ld. Continuing regardless\n",
				PTR_ERR(oom_reaper_th));
		oom_reaper_th = NULL;
	}
	return 0;
}