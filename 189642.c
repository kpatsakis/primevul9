static int __wmi_send(struct wil6210_priv *wil, u16 cmdid, void *buf, u16 len)
{
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wmi_cmd_hdr wmi;
	} __packed cmd = {
		.hdr = {
			.type = WIL_MBOX_HDR_TYPE_WMI,
			.flags = 0,
			.len = cpu_to_le16(sizeof(cmd.wmi) + len),
		},
		.wmi = {
			.mid = 0,
			.command_id = cpu_to_le16(cmdid),
		},
	};
	struct wil6210_mbox_ring *r = &wil->mbox_ctl.tx;
	struct wil6210_mbox_ring_desc d_head;
	u32 next_head;
	void __iomem *dst;
	void __iomem *head = wmi_addr(wil, r->head);
	uint retry;
	int rc = 0;

	if (len > r->entry_size - sizeof(cmd)) {
		wil_err(wil, "WMI size too large: %d bytes, max is %d\n",
			(int)(sizeof(cmd) + len), r->entry_size);
		return -ERANGE;
	}

	might_sleep();

	if (!test_bit(wil_status_fwready, wil->status)) {
		wil_err(wil, "WMI: cannot send command while FW not ready\n");
		return -EAGAIN;
	}

	/* Allow sending only suspend / resume commands during susepnd flow */
	if ((test_bit(wil_status_suspending, wil->status) ||
	     test_bit(wil_status_suspended, wil->status) ||
	     test_bit(wil_status_resuming, wil->status)) &&
	     ((cmdid != WMI_TRAFFIC_SUSPEND_CMDID) &&
	      (cmdid != WMI_TRAFFIC_RESUME_CMDID))) {
		wil_err(wil, "WMI: reject send_command during suspend\n");
		return -EINVAL;
	}

	if (!head) {
		wil_err(wil, "WMI head is garbage: 0x%08x\n", r->head);
		return -EINVAL;
	}

	wil_halp_vote(wil);

	/* read Tx head till it is not busy */
	for (retry = 5; retry > 0; retry--) {
		wil_memcpy_fromio_32(&d_head, head, sizeof(d_head));
		if (d_head.sync == 0)
			break;
		msleep(20);
	}
	if (d_head.sync != 0) {
		wil_err(wil, "WMI head busy\n");
		rc = -EBUSY;
		goto out;
	}
	/* next head */
	next_head = r->base + ((r->head - r->base + sizeof(d_head)) % r->size);
	wil_dbg_wmi(wil, "Head 0x%08x -> 0x%08x\n", r->head, next_head);
	/* wait till FW finish with previous command */
	for (retry = 5; retry > 0; retry--) {
		if (!test_bit(wil_status_fwready, wil->status)) {
			wil_err(wil, "WMI: cannot send command while FW not ready\n");
			rc = -EAGAIN;
			goto out;
		}
		r->tail = wil_r(wil, RGF_MBOX +
				offsetof(struct wil6210_mbox_ctl, tx.tail));
		if (next_head != r->tail)
			break;
		msleep(20);
	}
	if (next_head == r->tail) {
		wil_err(wil, "WMI ring full\n");
		rc = -EBUSY;
		goto out;
	}
	dst = wmi_buffer(wil, d_head.addr);
	if (!dst) {
		wil_err(wil, "invalid WMI buffer: 0x%08x\n",
			le32_to_cpu(d_head.addr));
		rc = -EAGAIN;
		goto out;
	}
	cmd.hdr.seq = cpu_to_le16(++wil->wmi_seq);
	/* set command */
	wil_dbg_wmi(wil, "sending %s (0x%04x) [%d]\n",
		    cmdid2name(cmdid), cmdid, len);
	wil_hex_dump_wmi("Cmd ", DUMP_PREFIX_OFFSET, 16, 1, &cmd,
			 sizeof(cmd), true);
	wil_hex_dump_wmi("cmd ", DUMP_PREFIX_OFFSET, 16, 1, buf,
			 len, true);
	wil_memcpy_toio_32(dst, &cmd, sizeof(cmd));
	wil_memcpy_toio_32(dst + sizeof(cmd), buf, len);
	/* mark entry as full */
	wil_w(wil, r->head + offsetof(struct wil6210_mbox_ring_desc, sync), 1);
	/* advance next ptr */
	wil_w(wil, RGF_MBOX + offsetof(struct wil6210_mbox_ctl, tx.head),
	      r->head = next_head);

	trace_wil6210_wmi_cmd(&cmd.wmi, buf, len);

	/* interrupt to FW */
	wil_w(wil, RGF_USER_USER_ICR + offsetof(struct RGF_ICR, ICS),
	      SW_INT_MBOX);

out:
	wil_halp_unvote(wil);
	return rc;
}