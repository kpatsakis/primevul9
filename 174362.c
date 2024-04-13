cssp_gss_report_error(OM_uint32 code, char *str, OM_uint32 major_status, OM_uint32 minor_status)
{
	OM_uint32 msgctx = 0, ms;
	gss_buffer_desc status_string;

	error("GSS error [%d:%d:%d]: %s\n", (major_status & 0xff000000) >> 24,	// Calling error
	      (major_status & 0xff0000) >> 16,	// Routine error
	      major_status & 0xffff,	// Supplementary info bits
	      str);

	do
	{
		ms = gss_display_status(&minor_status, major_status,
					code, GSS_C_NULL_OID, &msgctx, &status_string);
		if (ms != GSS_S_COMPLETE)
			continue;

		error(" - %s\n", status_string.value);

	}
	while (ms == GSS_S_COMPLETE && msgctx);

}