mISDN_ctrl(struct mISDNchannel *ch, u_int cmd, void *arg)
{
	struct mISDN_sock *msk;

	msk = container_of(ch, struct mISDN_sock, ch);
	if (*debug & DEBUG_SOCKET)
		printk(KERN_DEBUG "%s(%p, %x, %p)\n", __func__, ch, cmd, arg);
	switch (cmd) {
	case CLOSE_CHANNEL:
		msk->sk.sk_state = MISDN_CLOSED;
		break;
	}
	return 0;
}