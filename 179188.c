static int get_passwd_by_systemd(const char *prompt, char *input, int capacity)
{
	int fd[2];
	pid_t pid;
	int offs = 0;
	int rc = 1;

	if (pipe(fd) == -1) {
		fprintf(stderr, "Failed to create pipe: %s\n", strerror(errno));
		return 1;
	}

	pid = fork();
	if (pid == -1) {
		fprintf(stderr, "Unable to fork: %s\n", strerror(errno));
		close(fd[0]);
		close(fd[1]);
		return 1;
	}
	if (pid == 0) {
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		if (execlp("systemd-ask-password", "systemd-ask-password", prompt, NULL) == -1) {
			fprintf(stderr, "Failed to execute systemd-ask-password: %s\n",
				strerror(errno));
		}
		exit(1);
	}

	close(fd[1]);
	for (;;) {
		if (offs+1 >= capacity) {
			fprintf(stderr, "Password too long.\n");
			kill(pid, SIGTERM);
			rc = 1;
			break;
		}
		rc = read(fd[0], input + offs, capacity - offs);
		if (rc == -1) {
			fprintf(stderr, "Failed to read from pipe: %s\n", strerror(errno));
			rc = 1;
			break;
		}
		if (!rc)
			break;
		offs += rc;
		input[offs] = '\0';
	}
	if (wait(&rc) == -1) {
		fprintf(stderr, "Failed to wait child: %s\n", strerror(errno));
		rc = 1;
		goto out;
	}
	if (!WIFEXITED(rc) || WEXITSTATUS(rc)) {
		rc = 1;
		goto out;
	}

	rc = 0;

out:
	close(fd[0]);
	return rc;
}