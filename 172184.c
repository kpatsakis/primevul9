static int hidp_session_thread(void *arg)
{
	struct hidp_session *session = arg;
	DEFINE_WAIT_FUNC(ctrl_wait, hidp_session_wake_function);
	DEFINE_WAIT_FUNC(intr_wait, hidp_session_wake_function);

	BT_DBG("session %p", session);

	/* initialize runtime environment */
	hidp_session_get(session);
	__module_get(THIS_MODULE);
	set_user_nice(current, -15);
	hidp_set_timer(session);

	add_wait_queue(sk_sleep(session->ctrl_sock->sk), &ctrl_wait);
	add_wait_queue(sk_sleep(session->intr_sock->sk), &intr_wait);
	/* This memory barrier is paired with wq_has_sleeper(). See
	 * sock_poll_wait() for more information why this is needed. */
	smp_mb();

	/* notify synchronous startup that we're ready */
	atomic_inc(&session->state);
	wake_up(&session->state_queue);

	/* run session */
	hidp_session_run(session);

	/* cleanup runtime environment */
	remove_wait_queue(sk_sleep(session->intr_sock->sk), &intr_wait);
	remove_wait_queue(sk_sleep(session->intr_sock->sk), &ctrl_wait);
	wake_up_interruptible(&session->report_queue);
	hidp_del_timer(session);

	/*
	 * If we stopped ourself due to any internal signal, we should try to
	 * unregister our own session here to avoid having it linger until the
	 * parent l2cap_conn dies or user-space cleans it up.
	 * This does not deadlock as we don't do any synchronous shutdown.
	 * Instead, this call has the same semantics as if user-space tried to
	 * delete the session.
	 */
	l2cap_unregister_user(session->conn, &session->user);
	hidp_session_put(session);

	module_put_and_exit(0);
	return 0;
}