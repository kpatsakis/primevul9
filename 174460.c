cssp_gss_get_service_name(char *server, gss_name_t * name)
{
	gss_buffer_desc output;
	OM_uint32 major_status, minor_status;

	const char service_name[] = "TERMSRV";

	gss_OID type = (gss_OID) GSS_C_NT_HOSTBASED_SERVICE;
	int size = (strlen(service_name) + 1 + strlen(server) + 1);

	output.value = malloc(size);
	snprintf(output.value, size, "%s@%s", service_name, server);
	output.length = strlen(output.value) + 1;

	major_status = gss_import_name(&minor_status, &output, type, name);

	if (GSS_ERROR(major_status))
	{
		cssp_gss_report_error(GSS_C_GSS_CODE, "Failed to create service principal name",
				      major_status, minor_status);
		return False;
	}

	gss_release_buffer(&minor_status, &output);

	return True;

}