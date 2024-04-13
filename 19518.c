static const char* video_command_name(BYTE cmd)
{
	switch (cmd)
	{
		case TSMM_START_PRESENTATION:
			return "start";
		case TSMM_STOP_PRESENTATION:
			return "stop";
		default:
			return "<unknown>";
	}
}