static int __init oom_init(void)
{
	oom_reaper_th = kthread_run(oom_reaper, NULL, "oom_reaper");
	return 0;
}