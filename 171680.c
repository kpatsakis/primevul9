int __sock_cmsg_send(struct sock *sk, struct msghdr *msg, struct cmsghdr *cmsg,
		     struct sockcm_cookie *sockc)
{
	u32 tsflags;

	switch (cmsg->cmsg_type) {
	case SO_MARK:
		if (!ns_capable(sock_net(sk)->user_ns, CAP_NET_ADMIN))
			return -EPERM;
		if (cmsg->cmsg_len != CMSG_LEN(sizeof(u32)))
			return -EINVAL;
		sockc->mark = *(u32 *)CMSG_DATA(cmsg);
		break;
	case SO_TIMESTAMPING:
		if (cmsg->cmsg_len != CMSG_LEN(sizeof(u32)))
			return -EINVAL;

		tsflags = *(u32 *)CMSG_DATA(cmsg);
		if (tsflags & ~SOF_TIMESTAMPING_TX_RECORD_MASK)
			return -EINVAL;

		sockc->tsflags &= ~SOF_TIMESTAMPING_TX_RECORD_MASK;
		sockc->tsflags |= tsflags;
		break;
	/* SCM_RIGHTS and SCM_CREDENTIALS are semantically in SOL_UNIX. */
	case SCM_RIGHTS:
	case SCM_CREDENTIALS:
		break;
	default:
		return -EINVAL;
	}
	return 0;
}