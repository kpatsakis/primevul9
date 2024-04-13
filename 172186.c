static void hidp_session_run(struct hidp_session *session)
{
	struct sock *ctrl_sk = session->ctrl_sock->sk;
	struct sock *intr_sk = session->intr_sock->sk;
	struct sk_buff *skb;
	DEFINE_WAIT_FUNC(wait, woken_wake_function);

	add_wait_queue(&hidp_session_wq, &wait);
	for (;;) {
		/*
		 * This thread can be woken up two ways:
		 *  - You call hidp_session_terminate() which sets the
		 *    session->terminate flag and wakes this thread up.
		 *  - Via modifying the socket state of ctrl/intr_sock. This
		 *    thread is woken up by ->sk_state_changed().
		 */

		/* Ensure session->terminate is updated */
		smp_mb__before_atomic();
		if (atomic_read(&session->terminate))
			break;

		if (ctrl_sk->sk_state != BT_CONNECTED ||
		    intr_sk->sk_state != BT_CONNECTED)
			break;

		/* parse incoming intr-skbs */
		while ((skb = skb_dequeue(&intr_sk->sk_receive_queue))) {
			skb_orphan(skb);
			if (!skb_linearize(skb))
				hidp_recv_intr_frame(session, skb);
			else
				kfree_skb(skb);
		}

		/* send pending intr-skbs */
		hidp_process_transmit(session, &session->intr_transmit,
				      session->intr_sock);

		/* parse incoming ctrl-skbs */
		while ((skb = skb_dequeue(&ctrl_sk->sk_receive_queue))) {
			skb_orphan(skb);
			if (!skb_linearize(skb))
				hidp_recv_ctrl_frame(session, skb);
			else
				kfree_skb(skb);
		}

		/* send pending ctrl-skbs */
		hidp_process_transmit(session, &session->ctrl_transmit,
				      session->ctrl_sock);

		wait_woken(&wait, TASK_INTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT);
	}
	remove_wait_queue(&hidp_session_wq, &wait);

	atomic_inc(&session->terminate);

	/* Ensure session->terminate is updated */
	smp_mb__after_atomic();
}