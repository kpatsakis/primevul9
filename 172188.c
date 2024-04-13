static void hidp_copy_session(struct hidp_session *session, struct hidp_conninfo *ci)
{
	u32 valid_flags = 0;
	memset(ci, 0, sizeof(*ci));
	bacpy(&ci->bdaddr, &session->bdaddr);

	ci->flags = session->flags & valid_flags;
	ci->state = BT_CONNECTED;

	if (session->input) {
		ci->vendor  = session->input->id.vendor;
		ci->product = session->input->id.product;
		ci->version = session->input->id.version;
		if (session->input->name)
			strlcpy(ci->name, session->input->name, 128);
		else
			strlcpy(ci->name, "HID Boot Device", 128);
	} else if (session->hid) {
		ci->vendor  = session->hid->vendor;
		ci->product = session->hid->product;
		ci->version = session->hid->version;
		strlcpy(ci->name, session->hid->name, 128);
	}
}