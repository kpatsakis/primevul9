static int skcipher_recvmsg(struct socket *sock, struct msghdr *msg,
			    size_t ignored, int flags)
{
	return (msg->msg_iocb && !is_sync_kiocb(msg->msg_iocb)) ?
		skcipher_recvmsg_async(sock, msg, flags) :
		skcipher_recvmsg_sync(sock, msg, flags);
}