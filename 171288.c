send_vhost_message(int sockfd, struct VhostUserMsg *msg)
{
	if (!msg)
		return 0;

	return send_fd_message(sockfd, (char *)msg,
		VHOST_USER_HDR_SIZE + msg->size, NULL, 0);
}