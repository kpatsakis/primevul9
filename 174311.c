cssp_gss_unwrap(gss_ctx_id_t * ctx, STREAM in, STREAM out)
{
	OM_uint32 major_status;
	OM_uint32 minor_status;
	gss_qop_t qop_state;
	gss_buffer_desc inbuf, outbuf;
	int conf_state;

	inbuf.value = in->data;
	inbuf.length = s_length(in);

	major_status = gss_unwrap(&minor_status, ctx, &inbuf, &outbuf, &conf_state, &qop_state);

	if (major_status != GSS_S_COMPLETE)
	{
		cssp_gss_report_error(GSS_C_GSS_CODE, "Failed to decrypt message",
				      major_status, minor_status);
		return False;
	}

	out->data = out->p = xmalloc(outbuf.length);
	out->size = outbuf.length;
	out_uint8p(out, outbuf.value, outbuf.length);
	s_mark_end(out);

	gss_release_buffer(&minor_status, &outbuf);

	return True;
}