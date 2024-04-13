cssp_gss_mech_available(gss_OID mech)
{
	int mech_found;
	OM_uint32 major_status, minor_status;
	gss_OID_set mech_set;

	mech_found = 0;

	if (mech == GSS_C_NO_OID)
		return True;

	major_status = gss_indicate_mechs(&minor_status, &mech_set);
	if (!mech_set)
		return False;

	if (GSS_ERROR(major_status))
	{
		cssp_gss_report_error(GSS_C_GSS_CODE, "Failed to get available mechs on system",
				      major_status, minor_status);
		return False;
	}

	gss_test_oid_set_member(&minor_status, mech, mech_set, &mech_found);

	if (GSS_ERROR(major_status))
	{
		cssp_gss_report_error(GSS_C_GSS_CODE, "Failed to match mechanism in set",
				      major_status, minor_status);
		return False;
	}

	if (!mech_found)
		return False;

	return True;
}