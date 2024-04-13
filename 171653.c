static void sock_warn_obsolete_bsdism(const char *name)
{
	static int warned;
	static char warncomm[TASK_COMM_LEN];
	if (strcmp(warncomm, current->comm) && warned < 5) {
		strcpy(warncomm,  current->comm);
		pr_warn("process `%s' is using obsolete %s SO_BSDCOMPAT\n",
			warncomm, name);
		warned++;
	}
}