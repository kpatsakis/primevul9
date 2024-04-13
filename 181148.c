void ipc_print_size(int unit, char *msg, uint64_t size, const char *end,
		    int width)
{
	char format[32];

	if (!msg)
		/* NULL */ ;
	else if (msg[strlen(msg) - 1] == '=')
		printf("%s", msg);
	else if (unit == IPC_UNIT_BYTES)
		printf(_("%s (bytes) = "), msg);
	else if (unit == IPC_UNIT_KB)
		printf(_("%s (kbytes) = "), msg);
	else
		printf("%s = ", msg);

	switch (unit) {
	case IPC_UNIT_DEFAULT:
	case IPC_UNIT_BYTES:
		sprintf(format, "%%%dju", width);
		printf(format, size);
		break;
	case IPC_UNIT_KB:
		sprintf(format, "%%%dju", width);
		printf(format, size / 1024);
		break;
	case IPC_UNIT_HUMAN:
	{
		char *tmp;
		sprintf(format, "%%%ds", width);
		printf(format, (tmp = size_to_human_string(SIZE_SUFFIX_1LETTER, size)));
		free(tmp);
		break;
	}
	default:
		/* impossible occurred */
		abort();
	}

	if (end)
		printf("%s", end);
}