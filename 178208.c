static void update_oom_score_adj(char *data, size_t len)
{
	if (data == NULL || len <= 0)
		return;

	if (write_file(data, len, "/proc/self/oom_score_adj") < 0)
		bail("failed to update /proc/self/oom_score_adj");
}