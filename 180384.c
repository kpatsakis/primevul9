static const char *dccp_state_name(const int state)
{
	static const char *const dccp_state_names[] = {
	[DCCP_OPEN]		= "OPEN",
	[DCCP_REQUESTING]	= "REQUESTING",
	[DCCP_PARTOPEN]		= "PARTOPEN",
	[DCCP_LISTEN]		= "LISTEN",
	[DCCP_RESPOND]		= "RESPOND",
	[DCCP_CLOSING]		= "CLOSING",
	[DCCP_ACTIVE_CLOSEREQ]	= "CLOSEREQ",
	[DCCP_PASSIVE_CLOSE]	= "PASSIVE_CLOSE",
	[DCCP_PASSIVE_CLOSEREQ]	= "PASSIVE_CLOSEREQ",
	[DCCP_TIME_WAIT]	= "TIME_WAIT",
	[DCCP_CLOSED]		= "CLOSED",
	};

	if (state >= DCCP_MAX_STATES)
		return "INVALID STATE!";
	else
		return dccp_state_names[state];
}