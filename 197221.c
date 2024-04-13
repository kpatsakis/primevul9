int shadow_server_parse_command_line(rdpShadowServer* server, int argc, char** argv)
{
	int status;
	DWORD flags;
	COMMAND_LINE_ARGUMENT_A* arg;
	rdpSettings* settings = server->settings;
	COMMAND_LINE_ARGUMENT_A largs[ARRAYSIZE(shadow_args)];
	memcpy(largs, shadow_args, sizeof(shadow_args));

	if (argc < 2)
		return 1;

	CommandLineClearArgumentsA(largs);
	flags = COMMAND_LINE_SEPARATOR_COLON;
	flags |= COMMAND_LINE_SIGIL_SLASH | COMMAND_LINE_SIGIL_PLUS_MINUS;
	status = CommandLineParseArgumentsA(argc, argv, largs, flags, server, NULL, NULL);

	if (status < 0)
		return status;

	arg = largs;
	errno = 0;

	do
	{
		if (!(arg->Flags & COMMAND_LINE_ARGUMENT_PRESENT))
			continue;

		CommandLineSwitchStart(arg) CommandLineSwitchCase(arg, "port")
		{
			long val = strtol(arg->Value, NULL, 0);

			if ((errno != 0) || (val <= 0) || (val > UINT16_MAX))
				return -1;

			server->port = (DWORD)val;
		}
		CommandLineSwitchCase(arg, "ipc-socket")
		{
			/* /bind-address is incompatible */
			if (server->ipcSocket)
				return -1;

			server->ipcSocket = _strdup(arg->Value);

			if (!server->ipcSocket)
				return -1;
		}
		CommandLineSwitchCase(arg, "bind-address")
		{
			int rc;
			size_t len = strlen(arg->Value) + sizeof(bind_address);
			/* /ipc-socket is incompatible */
			if (server->ipcSocket)
				return -1;
			server->ipcSocket = calloc(len, sizeof(CHAR));

			if (!server->ipcSocket)
				return -1;

			rc = _snprintf(server->ipcSocket, len, "%s%s", bind_address, arg->Value);
			if ((rc < 0) || ((size_t)rc != len - 1))
				return -1;
		}
		CommandLineSwitchCase(arg, "may-view")
		{
			server->mayView = arg->Value ? TRUE : FALSE;
		}
		CommandLineSwitchCase(arg, "may-interact")
		{
			server->mayInteract = arg->Value ? TRUE : FALSE;
		}
		CommandLineSwitchCase(arg, "rect")
		{
			char* p;
			char* tok[4];
			long x = -1, y = -1, w = -1, h = -1;
			char* str = _strdup(arg->Value);

			if (!str)
				return -1;

			tok[0] = p = str;
			p = strchr(p + 1, ',');

			if (!p)
			{
				free(str);
				return -1;
			}

			*p++ = '\0';
			tok[1] = p;
			p = strchr(p + 1, ',');

			if (!p)
			{
				free(str);
				return -1;
			}

			*p++ = '\0';
			tok[2] = p;
			p = strchr(p + 1, ',');

			if (!p)
			{
				free(str);
				return -1;
			}

			*p++ = '\0';
			tok[3] = p;
			x = strtol(tok[0], NULL, 0);

			if (errno != 0)
				goto fail;

			y = strtol(tok[1], NULL, 0);

			if (errno != 0)
				goto fail;

			w = strtol(tok[2], NULL, 0);

			if (errno != 0)
				goto fail;

			h = strtol(tok[3], NULL, 0);

			if (errno != 0)
				goto fail;

		fail:
			free(str);

			if ((x < 0) || (y < 0) || (w < 1) || (h < 1) || (errno != 0))
				return -1;

			server->subRect.left = x;
			server->subRect.top = y;
			server->subRect.right = x + w;
			server->subRect.bottom = y + h;
			server->shareSubRect = TRUE;
		}
		CommandLineSwitchCase(arg, "auth")
		{
			server->authentication = arg->Value ? TRUE : FALSE;
		}
		CommandLineSwitchCase(arg, "sec")
		{
			if (strcmp("rdp", arg->Value) == 0) /* Standard RDP */
			{
				settings->RdpSecurity = TRUE;
				settings->TlsSecurity = FALSE;
				settings->NlaSecurity = FALSE;
				settings->ExtSecurity = FALSE;
				settings->UseRdpSecurityLayer = TRUE;
			}
			else if (strcmp("tls", arg->Value) == 0) /* TLS */
			{
				settings->RdpSecurity = FALSE;
				settings->TlsSecurity = TRUE;
				settings->NlaSecurity = FALSE;
				settings->ExtSecurity = FALSE;
			}
			else if (strcmp("nla", arg->Value) == 0) /* NLA */
			{
				settings->RdpSecurity = FALSE;
				settings->TlsSecurity = FALSE;
				settings->NlaSecurity = TRUE;
				settings->ExtSecurity = FALSE;
			}
			else if (strcmp("ext", arg->Value) == 0) /* NLA Extended */
			{
				settings->RdpSecurity = FALSE;
				settings->TlsSecurity = FALSE;
				settings->NlaSecurity = FALSE;
				settings->ExtSecurity = TRUE;
			}
			else
			{
				WLog_ERR(TAG, "unknown protocol security: %s", arg->Value);
			}
		}
		CommandLineSwitchCase(arg, "sec-rdp")
		{
			settings->RdpSecurity = arg->Value ? TRUE : FALSE;
		}
		CommandLineSwitchCase(arg, "sec-tls")
		{
			settings->TlsSecurity = arg->Value ? TRUE : FALSE;
		}
		CommandLineSwitchCase(arg, "sec-nla")
		{
			settings->NlaSecurity = arg->Value ? TRUE : FALSE;
		}
		CommandLineSwitchCase(arg, "sec-ext")
		{
			settings->ExtSecurity = arg->Value ? TRUE : FALSE;
		}
		CommandLineSwitchCase(arg, "sam-file")
		{
			freerdp_settings_set_string(settings, FreeRDP_NtlmSamFile, arg->Value);
		}
		CommandLineSwitchDefault(arg)
		{
		}
		CommandLineSwitchEnd(arg)
	} while ((arg = CommandLineFindNextArgumentA(arg)) != NULL);

	arg = CommandLineFindArgumentA(largs, "monitors");

	if (arg && (arg->Flags & COMMAND_LINE_ARGUMENT_PRESENT))
	{
		int index;
		int numMonitors;
		MONITOR_DEF monitors[16];
		numMonitors = shadow_enum_monitors(monitors, 16);

		if (arg->Flags & COMMAND_LINE_VALUE_PRESENT)
		{
			/* Select monitors */
			long val = strtol(arg->Value, NULL, 0);

			if ((val < 0) || (errno != 0) || (val >= numMonitors))
				status = COMMAND_LINE_STATUS_PRINT;

			server->selectedMonitor = val;
		}
		else
		{
			int width, height;
			MONITOR_DEF* monitor;

			/* List monitors */

			for (index = 0; index < numMonitors; index++)
			{
				monitor = &monitors[index];
				width = monitor->right - monitor->left;
				height = monitor->bottom - monitor->top;
				WLog_INFO(TAG, "      %s [%d] %dx%d\t+%" PRId32 "+%" PRId32 "",
				          (monitor->flags == 1) ? "*" : " ", index, width, height, monitor->left,
				          monitor->top);
			}

			status = COMMAND_LINE_STATUS_PRINT;
		}
	}

	return status;
}