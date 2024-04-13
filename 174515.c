cssp_gss_wrap(gss_ctx_id_t * ctx, STREAM in, STREAM out)
{
	int conf_state;
	OM_uint32 major_status;
	OM_uint32 minor_status;
	gss_buffer_desc inbuf, outbuf;

	inbuf.value = in->data;
	inbuf.length = s_length(in);

	major_status = gss_wrap(&minor_status, ctx, True,
				GSS_C_QOP_DEFAULT, &inbuf, &conf_state, &outbuf);

	if (major_status != GSS_S_COMPLETE)
	{
		cssp_gss_report_error(GSS_C_GSS_CODE, "Failed to encrypt and sign message",
				      major_status, minor_status);
		return False;
	}

	if (!conf_state)
	{
		error("GSS Confidentiality failed, no encryption of message performed.");
		return False;
	}

	// write enc data to out stream
	out->data = out->p = xmalloc(outbuf.length);
	out->size = outbuf.length;
	out_uint8p(out, outbuf.value, outbuf.length);
	s_mark_end(out);

	gss_release_buffer(&minor_status, &outbuf);

	return True;
}