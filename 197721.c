static int cgw_put_job(struct sk_buff *skb, struct cgw_job *gwj, int type,
		       u32 pid, u32 seq, int flags)
{
	struct cgw_frame_mod mb;
	struct rtcanmsg *rtcan;
	struct nlmsghdr *nlh;

	nlh = nlmsg_put(skb, pid, seq, type, sizeof(*rtcan), flags);
	if (!nlh)
		return -EMSGSIZE;

	rtcan = nlmsg_data(nlh);
	rtcan->can_family = AF_CAN;
	rtcan->gwtype = gwj->gwtype;
	rtcan->flags = gwj->flags;

	/* add statistics if available */

	if (gwj->handled_frames) {
		if (nla_put_u32(skb, CGW_HANDLED, gwj->handled_frames) < 0)
			goto cancel;
	}

	if (gwj->dropped_frames) {
		if (nla_put_u32(skb, CGW_DROPPED, gwj->dropped_frames) < 0)
			goto cancel;
	}

	if (gwj->deleted_frames) {
		if (nla_put_u32(skb, CGW_DELETED, gwj->deleted_frames) < 0)
			goto cancel;
	}

	/* check non default settings of attributes */

	if (gwj->limit_hops) {
		if (nla_put_u8(skb, CGW_LIM_HOPS, gwj->limit_hops) < 0)
			goto cancel;
	}

	if (gwj->mod.modtype.and) {
		memcpy(&mb.cf, &gwj->mod.modframe.and, sizeof(mb.cf));
		mb.modtype = gwj->mod.modtype.and;
		if (nla_put(skb, CGW_MOD_AND, sizeof(mb), &mb) < 0)
			goto cancel;
	}

	if (gwj->mod.modtype.or) {
		memcpy(&mb.cf, &gwj->mod.modframe.or, sizeof(mb.cf));
		mb.modtype = gwj->mod.modtype.or;
		if (nla_put(skb, CGW_MOD_OR, sizeof(mb), &mb) < 0)
			goto cancel;
	}

	if (gwj->mod.modtype.xor) {
		memcpy(&mb.cf, &gwj->mod.modframe.xor, sizeof(mb.cf));
		mb.modtype = gwj->mod.modtype.xor;
		if (nla_put(skb, CGW_MOD_XOR, sizeof(mb), &mb) < 0)
			goto cancel;
	}

	if (gwj->mod.modtype.set) {
		memcpy(&mb.cf, &gwj->mod.modframe.set, sizeof(mb.cf));
		mb.modtype = gwj->mod.modtype.set;
		if (nla_put(skb, CGW_MOD_SET, sizeof(mb), &mb) < 0)
			goto cancel;
	}

	if (gwj->mod.uid) {
		if (nla_put_u32(skb, CGW_MOD_UID, gwj->mod.uid) < 0)
			goto cancel;
	}

	if (gwj->mod.csumfunc.crc8) {
		if (nla_put(skb, CGW_CS_CRC8, CGW_CS_CRC8_LEN,
			    &gwj->mod.csum.crc8) < 0)
			goto cancel;
	}

	if (gwj->mod.csumfunc.xor) {
		if (nla_put(skb, CGW_CS_XOR, CGW_CS_XOR_LEN,
			    &gwj->mod.csum.xor) < 0)
			goto cancel;
	}

	if (gwj->gwtype == CGW_TYPE_CAN_CAN) {

		if (gwj->ccgw.filter.can_id || gwj->ccgw.filter.can_mask) {
			if (nla_put(skb, CGW_FILTER, sizeof(struct can_filter),
				    &gwj->ccgw.filter) < 0)
				goto cancel;
		}

		if (nla_put_u32(skb, CGW_SRC_IF, gwj->ccgw.src_idx) < 0)
			goto cancel;

		if (nla_put_u32(skb, CGW_DST_IF, gwj->ccgw.dst_idx) < 0)
			goto cancel;
	}

	nlmsg_end(skb, nlh);
	return 0;

cancel:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}