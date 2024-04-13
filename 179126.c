void sigchld_handler(int s) {
        int* status=NULL;
	int* i;
	pid_t pid;
	int done=0;

	while(!done && (pid=wait(status)) > 0) {
		if(WIFEXITED(status)) {
			msg3(LOG_INFO, "Child exited with %d", WEXITSTATUS(status));
			msg3(LOG_INFO, "pid is %d", pid);
			done=1;
		}
		i=g_hash_table_lookup(children, &pid);
		if(!i) {
			msg3(LOG_INFO, "SIGCHLD received for an unknown child with PID %ld", (long)pid);
		} else {
			DEBUG2("Removing %d from the list of children", pid);
			g_hash_table_remove(children, &pid);
		}
	}
}