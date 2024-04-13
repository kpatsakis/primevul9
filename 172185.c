int hidp_connection_del(struct hidp_conndel_req *req)
{
	u32 valid_flags = BIT(HIDP_VIRTUAL_CABLE_UNPLUG);
	struct hidp_session *session;

	if (req->flags & ~valid_flags)
		return -EINVAL;

	session = hidp_session_find(&req->bdaddr);
	if (!session)
		return -ENOENT;

	if (req->flags & BIT(HIDP_VIRTUAL_CABLE_UNPLUG))
		hidp_send_ctrl_message(session,
				       HIDP_TRANS_HID_CONTROL |
				         HIDP_CTRL_VIRTUAL_CABLE_UNPLUG,
				       NULL, 0);
	else
		l2cap_unregister_user(session->conn, &session->user);

	hidp_session_put(session);

	return 0;
}