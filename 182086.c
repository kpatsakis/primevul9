static struct child_process *git_tcp_connect(int fd[2], char *host, int flags)
{
	int sockfd = git_tcp_connect_sock(host, flags);

	fd[0] = sockfd;
	fd[1] = dup(sockfd);

	return &no_fork;
}