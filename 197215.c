static int shadow_server_print_command_line_help(int argc, char** argv)
{
	char* str;
	size_t length;
	COMMAND_LINE_ARGUMENT_A* arg;
	COMMAND_LINE_ARGUMENT_A largs[ARRAYSIZE(shadow_args)];
	memcpy(largs, shadow_args, sizeof(shadow_args));
	if (argc < 1)
		return -1;

	WLog_INFO(TAG, "Usage: %s [options]", argv[0]);
	WLog_INFO(TAG, "");
	WLog_INFO(TAG, "Syntax:");
	WLog_INFO(TAG, "    /flag (enables flag)");
	WLog_INFO(TAG, "    /option:<value> (specifies option with value)");
	WLog_INFO(TAG,
	          "    +toggle -toggle (enables or disables toggle, where '/' is a synonym of '+')");
	WLog_INFO(TAG, "");
	arg = largs;

	do
	{
		if (arg->Flags & COMMAND_LINE_VALUE_FLAG)
		{
			WLog_INFO(TAG, "    %s", "/");
			WLog_INFO(TAG, "%-20s", arg->Name);
			WLog_INFO(TAG, "\t%s", arg->Text);
		}
		else if ((arg->Flags & COMMAND_LINE_VALUE_REQUIRED) ||
		         (arg->Flags & COMMAND_LINE_VALUE_OPTIONAL))
		{
			WLog_INFO(TAG, "    %s", "/");

			if (arg->Format)
			{
				length = (strlen(arg->Name) + strlen(arg->Format) + 2);
				str = (char*)malloc(length + 1);

				if (!str)
					return -1;

				sprintf_s(str, length + 1, "%s:%s", arg->Name, arg->Format);
				WLog_INFO(TAG, "%-20s", str);
				free(str);
			}
			else
			{
				WLog_INFO(TAG, "%-20s", arg->Name);
			}

			WLog_INFO(TAG, "\t%s", arg->Text);
		}
		else if (arg->Flags & COMMAND_LINE_VALUE_BOOL)
		{
			length = strlen(arg->Name) + 32;
			str = (char*)malloc(length + 1);

			if (!str)
				return -1;

			sprintf_s(str, length + 1, "%s (default:%s)", arg->Name, arg->Default ? "on" : "off");
			WLog_INFO(TAG, "    %s", arg->Default ? "-" : "+");
			WLog_INFO(TAG, "%-20s", str);
			free(str);
			WLog_INFO(TAG, "\t%s", arg->Text);
		}
	} while ((arg = CommandLineFindNextArgumentA(arg)) != NULL);

	return 1;
}