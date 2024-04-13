int ensure_cloned_binary(void)
{
	int execfd;
	char **argv = NULL, **envp = NULL;

	/* Check that we're not self-cloned, and if we are then bail. */
	int cloned = is_self_cloned();
	if (cloned > 0 || cloned == -ENOTRECOVERABLE)
		return cloned;

	if (fetchve(&argv, &envp) < 0)
		return -EINVAL;

	execfd = clone_binary();
	if (execfd < 0)
		return -EIO;

	fexecve(execfd, argv, envp);
	return -ENOEXEC;
}