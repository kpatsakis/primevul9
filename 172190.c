static int hidp_session_new(struct hidp_session **out, const bdaddr_t *bdaddr,
			    struct socket *ctrl_sock,
			    struct socket *intr_sock,
			    struct hidp_connadd_req *req,
			    struct l2cap_conn *conn)
{
	struct hidp_session *session;
	int ret;
	struct bt_sock *ctrl, *intr;

	ctrl = bt_sk(ctrl_sock->sk);
	intr = bt_sk(intr_sock->sk);

	session = kzalloc(sizeof(*session), GFP_KERNEL);
	if (!session)
		return -ENOMEM;

	/* object and runtime management */
	kref_init(&session->ref);
	atomic_set(&session->state, HIDP_SESSION_IDLING);
	init_waitqueue_head(&session->state_queue);
	session->flags = req->flags & BIT(HIDP_BLUETOOTH_VENDOR_ID);

	/* connection management */
	bacpy(&session->bdaddr, bdaddr);
	session->conn = l2cap_conn_get(conn);
	session->user.probe = hidp_session_probe;
	session->user.remove = hidp_session_remove;
	INIT_LIST_HEAD(&session->user.list);
	session->ctrl_sock = ctrl_sock;
	session->intr_sock = intr_sock;
	skb_queue_head_init(&session->ctrl_transmit);
	skb_queue_head_init(&session->intr_transmit);
	session->ctrl_mtu = min_t(uint, l2cap_pi(ctrl)->chan->omtu,
					l2cap_pi(ctrl)->chan->imtu);
	session->intr_mtu = min_t(uint, l2cap_pi(intr)->chan->omtu,
					l2cap_pi(intr)->chan->imtu);
	session->idle_to = req->idle_to;

	/* device management */
	INIT_WORK(&session->dev_init, hidp_session_dev_work);
	timer_setup(&session->timer, hidp_idle_timeout, 0);

	/* session data */
	mutex_init(&session->report_mutex);
	init_waitqueue_head(&session->report_queue);

	ret = hidp_session_dev_init(session, req);
	if (ret)
		goto err_free;

	get_file(session->intr_sock->file);
	get_file(session->ctrl_sock->file);
	*out = session;
	return 0;

err_free:
	l2cap_conn_put(session->conn);
	kfree(session);
	return ret;
}