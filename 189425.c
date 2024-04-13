void ikev1_init_out_pbs_echo_hdr(struct msg_digest *md, bool enc,
				 pb_stream *output_stream, uint8_t *output_buffer,
				 size_t sizeof_output_buffer,
				 pb_stream *rbody)
{
	struct isakmp_hdr hdr = md->hdr; /* mostly same as incoming header */

	/* make sure we start with a clean buffer */
	init_out_pbs(output_stream, output_buffer, sizeof_output_buffer,
		     "reply packet");

	hdr.isa_flags = 0; /* zero all flags */
	if (enc)
		hdr.isa_flags |= ISAKMP_FLAGS_v1_ENCRYPTION;

	if (impair.send_bogus_isakmp_flag) {
		hdr.isa_flags |= ISAKMP_FLAGS_RESERVED_BIT6;
	}

	/* there is only one IKEv1 version, and no new one will ever come - no need to set version */
	hdr.isa_np = 0;
	/* surely must have room and be well-formed */
	passert(out_struct(&hdr, &isakmp_hdr_desc, output_stream, rbody));
}