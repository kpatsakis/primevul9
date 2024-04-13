static int cgw_parse_attr(struct nlmsghdr *nlh, struct cf_mod *mod,
			  u8 gwtype, void *gwtypeattr, u8 *limhops)
{
	struct nlattr *tb[CGW_MAX+1];
	struct cgw_frame_mod mb;
	int modidx = 0;
	int err = 0;

	/* initialize modification & checksum data space */
	memset(mod, 0, sizeof(*mod));

	err = nlmsg_parse(nlh, sizeof(struct rtcanmsg), tb, CGW_MAX,
			  cgw_policy, NULL);
	if (err < 0)
		return err;

	if (tb[CGW_LIM_HOPS]) {
		*limhops = nla_get_u8(tb[CGW_LIM_HOPS]);

		if (*limhops < 1 || *limhops > max_hops)
			return -EINVAL;
	}

	/* check for AND/OR/XOR/SET modifications */

	if (tb[CGW_MOD_AND]) {
		nla_memcpy(&mb, tb[CGW_MOD_AND], CGW_MODATTR_LEN);

		canframecpy(&mod->modframe.and, &mb.cf);
		mod->modtype.and = mb.modtype;

		if (mb.modtype & CGW_MOD_ID)
			mod->modfunc[modidx++] = mod_and_id;

		if (mb.modtype & CGW_MOD_DLC)
			mod->modfunc[modidx++] = mod_and_dlc;

		if (mb.modtype & CGW_MOD_DATA)
			mod->modfunc[modidx++] = mod_and_data;
	}

	if (tb[CGW_MOD_OR]) {
		nla_memcpy(&mb, tb[CGW_MOD_OR], CGW_MODATTR_LEN);

		canframecpy(&mod->modframe.or, &mb.cf);
		mod->modtype.or = mb.modtype;

		if (mb.modtype & CGW_MOD_ID)
			mod->modfunc[modidx++] = mod_or_id;

		if (mb.modtype & CGW_MOD_DLC)
			mod->modfunc[modidx++] = mod_or_dlc;

		if (mb.modtype & CGW_MOD_DATA)
			mod->modfunc[modidx++] = mod_or_data;
	}

	if (tb[CGW_MOD_XOR]) {
		nla_memcpy(&mb, tb[CGW_MOD_XOR], CGW_MODATTR_LEN);

		canframecpy(&mod->modframe.xor, &mb.cf);
		mod->modtype.xor = mb.modtype;

		if (mb.modtype & CGW_MOD_ID)
			mod->modfunc[modidx++] = mod_xor_id;

		if (mb.modtype & CGW_MOD_DLC)
			mod->modfunc[modidx++] = mod_xor_dlc;

		if (mb.modtype & CGW_MOD_DATA)
			mod->modfunc[modidx++] = mod_xor_data;
	}

	if (tb[CGW_MOD_SET]) {
		nla_memcpy(&mb, tb[CGW_MOD_SET], CGW_MODATTR_LEN);

		canframecpy(&mod->modframe.set, &mb.cf);
		mod->modtype.set = mb.modtype;

		if (mb.modtype & CGW_MOD_ID)
			mod->modfunc[modidx++] = mod_set_id;

		if (mb.modtype & CGW_MOD_DLC)
			mod->modfunc[modidx++] = mod_set_dlc;

		if (mb.modtype & CGW_MOD_DATA)
			mod->modfunc[modidx++] = mod_set_data;
	}

	/* check for checksum operations after CAN frame modifications */
	if (modidx) {

		if (tb[CGW_CS_CRC8]) {
			struct cgw_csum_crc8 *c = nla_data(tb[CGW_CS_CRC8]);

			err = cgw_chk_csum_parms(c->from_idx, c->to_idx,
						 c->result_idx);
			if (err)
				return err;

			nla_memcpy(&mod->csum.crc8, tb[CGW_CS_CRC8],
				   CGW_CS_CRC8_LEN);

			/*
			 * select dedicated processing function to reduce
			 * runtime operations in receive hot path.
			 */
			if (c->from_idx < 0 || c->to_idx < 0 ||
			    c->result_idx < 0)
				mod->csumfunc.crc8 = cgw_csum_crc8_rel;
			else if (c->from_idx <= c->to_idx)
				mod->csumfunc.crc8 = cgw_csum_crc8_pos;
			else
				mod->csumfunc.crc8 = cgw_csum_crc8_neg;
		}

		if (tb[CGW_CS_XOR]) {
			struct cgw_csum_xor *c = nla_data(tb[CGW_CS_XOR]);

			err = cgw_chk_csum_parms(c->from_idx, c->to_idx,
						 c->result_idx);
			if (err)
				return err;

			nla_memcpy(&mod->csum.xor, tb[CGW_CS_XOR],
				   CGW_CS_XOR_LEN);

			/*
			 * select dedicated processing function to reduce
			 * runtime operations in receive hot path.
			 */
			if (c->from_idx < 0 || c->to_idx < 0 ||
			    c->result_idx < 0)
				mod->csumfunc.xor = cgw_csum_xor_rel;
			else if (c->from_idx <= c->to_idx)
				mod->csumfunc.xor = cgw_csum_xor_pos;
			else
				mod->csumfunc.xor = cgw_csum_xor_neg;
		}

		if (tb[CGW_MOD_UID]) {
			nla_memcpy(&mod->uid, tb[CGW_MOD_UID], sizeof(u32));
		}
	}

	if (gwtype == CGW_TYPE_CAN_CAN) {

		/* check CGW_TYPE_CAN_CAN specific attributes */

		struct can_can_gw *ccgw = (struct can_can_gw *)gwtypeattr;
		memset(ccgw, 0, sizeof(*ccgw));

		/* check for can_filter in attributes */
		if (tb[CGW_FILTER])
			nla_memcpy(&ccgw->filter, tb[CGW_FILTER],
				   sizeof(struct can_filter));

		err = -ENODEV;

		/* specifying two interfaces is mandatory */
		if (!tb[CGW_SRC_IF] || !tb[CGW_DST_IF])
			return err;

		ccgw->src_idx = nla_get_u32(tb[CGW_SRC_IF]);
		ccgw->dst_idx = nla_get_u32(tb[CGW_DST_IF]);

		/* both indices set to 0 for flushing all routing entries */
		if (!ccgw->src_idx && !ccgw->dst_idx)
			return 0;

		/* only one index set to 0 is an error */
		if (!ccgw->src_idx || !ccgw->dst_idx)
			return err;
	}

	/* add the checks for other gwtypes here */

	return 0;
}