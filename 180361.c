void oom_killer_enable(void)
{
	oom_killer_disabled = false;
	pr_info("OOM killer enabled.\n");
}