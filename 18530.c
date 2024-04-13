write_pid_file(int pid)
{
	int fd = open(PIDFILE, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if (fd < 0) {
err:
		fprintf(stderr, "couldn't open pidfile: %m\n");
		exit(1);
	}
	char *pidstr = NULL;
	int rc = asprintf(&pidstr, "%d\n", pid);
	if (rc < 0)
		goto err;

	rc = write(fd, pidstr, strlen(pidstr)+1);
	if (rc < 0)
		goto err;

	free(pidstr);
	close(fd);
}