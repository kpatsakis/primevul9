int shadow_server_command_line_status_print(rdpShadowServer* server, int argc, char** argv,
                                            int status)
{
	WINPR_UNUSED(server);

	if (status == COMMAND_LINE_STATUS_PRINT_VERSION)
	{
		WLog_INFO(TAG, "FreeRDP version %s (git %s)", FREERDP_VERSION_FULL, GIT_REVISION);
		return COMMAND_LINE_STATUS_PRINT_VERSION;
	}
	else if (status == COMMAND_LINE_STATUS_PRINT_BUILDCONFIG)
	{
		WLog_INFO(TAG, "%s", freerdp_get_build_config());
		return COMMAND_LINE_STATUS_PRINT_BUILDCONFIG;
	}
	else if (status == COMMAND_LINE_STATUS_PRINT)
	{
		return COMMAND_LINE_STATUS_PRINT;
	}
	else if (status < 0)
	{
		if (shadow_server_print_command_line_help(argc, argv) < 0)
			return -1;

		return COMMAND_LINE_STATUS_PRINT_HELP;
	}

	return 1;
}