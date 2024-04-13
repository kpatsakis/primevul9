void wmi_recv_cmd(struct wil6210_priv *wil)
{
	struct wil6210_mbox_ring_desc d_tail;
	struct wil6210_mbox_hdr hdr;
	struct wil6210_mbox_ring *r = &wil->mbox_ctl.rx;
	struct pending_wmi_event *evt;
	u8 *cmd;
	void __iomem *src;
	ulong flags;
	unsigned n;
	unsigned int num_immed_reply = 0;

	if (!test_bit(wil_status_mbox_ready, wil->status)) {
		wil_err(wil, "Reset in progress. Cannot handle WMI event\n");
		return;
	}

	if (test_bit(wil_status_suspended, wil->status)) {
		wil_err(wil, "suspended. cannot handle WMI event\n");
		return;
	}

	for (n = 0;; n++) {
		u16 len;
		bool q;
		bool immed_reply = false;

		r->head = wil_r(wil, RGF_MBOX +
				offsetof(struct wil6210_mbox_ctl, rx.head));
		if (r->tail == r->head)
			break;

		wil_dbg_wmi(wil, "Mbox head %08x tail %08x\n",
			    r->head, r->tail);
		/* read cmd descriptor from tail */
		wil_memcpy_fromio_32(&d_tail, wil->csr + HOSTADDR(r->tail),
				     sizeof(struct wil6210_mbox_ring_desc));
		if (d_tail.sync == 0) {
			wil_err(wil, "Mbox evt not owned by FW?\n");
			break;
		}

		/* read cmd header from descriptor */
		if (0 != wmi_read_hdr(wil, d_tail.addr, &hdr)) {
			wil_err(wil, "Mbox evt at 0x%08x?\n",
				le32_to_cpu(d_tail.addr));
			break;
		}
		len = le16_to_cpu(hdr.len);
		wil_dbg_wmi(wil, "Mbox evt %04x %04x %04x %02x\n",
			    le16_to_cpu(hdr.seq), len, le16_to_cpu(hdr.type),
			    hdr.flags);

		/* read cmd buffer from descriptor */
		src = wmi_buffer(wil, d_tail.addr) +
		      sizeof(struct wil6210_mbox_hdr);
		evt = kmalloc(ALIGN(offsetof(struct pending_wmi_event,
					     event.wmi) + len, 4),
			      GFP_KERNEL);
		if (!evt)
			break;

		evt->event.hdr = hdr;
		cmd = (void *)&evt->event.wmi;
		wil_memcpy_fromio_32(cmd, src, len);
		/* mark entry as empty */
		wil_w(wil, r->tail +
		      offsetof(struct wil6210_mbox_ring_desc, sync), 0);
		/* indicate */
		if ((hdr.type == WIL_MBOX_HDR_TYPE_WMI) &&
		    (len >= sizeof(struct wmi_cmd_hdr))) {
			struct wmi_cmd_hdr *wmi = &evt->event.wmi;
			u16 id = le16_to_cpu(wmi->command_id);
			u32 tstamp = le32_to_cpu(wmi->fw_timestamp);
			if (test_bit(wil_status_resuming, wil->status)) {
				if (id == WMI_TRAFFIC_RESUME_EVENTID)
					clear_bit(wil_status_resuming,
						  wil->status);
				else
					wil_err(wil,
						"WMI evt %d while resuming\n",
						id);
			}
			spin_lock_irqsave(&wil->wmi_ev_lock, flags);
			if (wil->reply_id && wil->reply_id == id) {
				if (wil->reply_buf) {
					memcpy(wil->reply_buf, wmi,
					       min(len, wil->reply_size));
					immed_reply = true;
				}
				if (id == WMI_TRAFFIC_SUSPEND_EVENTID) {
					wil_dbg_wmi(wil,
						    "set suspend_resp_rcvd\n");
					wil->suspend_resp_rcvd = true;
				}
			}
			spin_unlock_irqrestore(&wil->wmi_ev_lock, flags);

			wil_dbg_wmi(wil, "recv %s (0x%04x) MID %d @%d msec\n",
				    eventid2name(id), id, wmi->mid, tstamp);
			trace_wil6210_wmi_event(wmi, &wmi[1],
						len - sizeof(*wmi));
		}
		wil_hex_dump_wmi("evt ", DUMP_PREFIX_OFFSET, 16, 1,
				 &evt->event.hdr, sizeof(hdr) + len, true);

		/* advance tail */
		r->tail = r->base + ((r->tail - r->base +
			  sizeof(struct wil6210_mbox_ring_desc)) % r->size);
		wil_w(wil, RGF_MBOX +
		      offsetof(struct wil6210_mbox_ctl, rx.tail), r->tail);

		if (immed_reply) {
			wil_dbg_wmi(wil, "recv_cmd: Complete WMI 0x%04x\n",
				    wil->reply_id);
			kfree(evt);
			num_immed_reply++;
			complete(&wil->wmi_call);
		} else {
			/* add to the pending list */
			spin_lock_irqsave(&wil->wmi_ev_lock, flags);
			list_add_tail(&evt->list, &wil->pending_wmi_ev);
			spin_unlock_irqrestore(&wil->wmi_ev_lock, flags);
			q = queue_work(wil->wmi_wq, &wil->wmi_event_worker);
			wil_dbg_wmi(wil, "queue_work -> %d\n", q);
		}
	}
	/* normally, 1 event per IRQ should be processed */
	wil_dbg_wmi(wil, "recv_cmd: -> %d events queued, %d completed\n",
		    n - num_immed_reply, num_immed_reply);
}