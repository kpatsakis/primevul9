static void hidp_idle_timeout(struct timer_list *t)
{
	struct hidp_session *session = from_timer(session, t, timer);

	/* The HIDP user-space API only contains calls to add and remove
	 * devices. There is no way to forward events of any kind. Therefore,
	 * we have to forcefully disconnect a device on idle-timeouts. This is
	 * unfortunate and weird API design, but it is spec-compliant and
	 * required for backwards-compatibility. Hence, on idle-timeout, we
	 * signal driver-detach events, so poll() will be woken up with an
	 * error-condition on both sockets.
	 */

	session->intr_sock->sk->sk_err = EUNATCH;
	session->ctrl_sock->sk->sk_err = EUNATCH;
	wake_up_interruptible(sk_sleep(session->intr_sock->sk));
	wake_up_interruptible(sk_sleep(session->ctrl_sock->sk));

	hidp_session_terminate(session);
}