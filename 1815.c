u32 gf_bs_read_ue_log_idx3(GF_BitStream *bs, const char *fname, s32 idx1, s32 idx2, s32 idx3)
{
	u32 val=0, code;
	s32 nb_lead = -1;
	u32 bits = 0;
	for (code=0; !code; nb_lead++) {
		if (nb_lead>=32) {
			break;
		}
		code = gf_bs_read_int(bs, 1);
		bits++;
	}

	if (nb_lead>=32) {
		//gf_bs_read_int keeps returning 0 on EOS, so if no more bits available, rbsp was truncated otherwise code is broken in rbsp)
		//we only test once nb_lead>=32 to avoid testing at each bit read
		if (!gf_bs_available(bs)) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[Core] exp-golomb read failed, not enough bits in bitstream !\n"));
		} else {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[Core] corrupted exp-golomb code, %d leading zeros, max 31 allowed !\n", nb_lead));
		}
		return 0;
	}

	if (nb_lead) {
		u32 leads=1;
		val = gf_bs_read_int(bs, nb_lead);
		leads <<= nb_lead;
		leads -= 1;
		val += leads;
//		val += (1 << nb_lead) - 1;
		bits += nb_lead;
	}

	if (fname) {
		gf_bs_log_idx(bs, bits, fname, val, idx1, idx2, idx3);
	}
	return val;
}