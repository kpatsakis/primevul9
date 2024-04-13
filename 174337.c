mcs_recv_cjcf(void)
{
	uint8 opcode, result;
	STREAM s;

	s = iso_recv(NULL);
	if (s == NULL)
		return False;

	in_uint8(s, opcode);
	if ((opcode >> 2) != MCS_CJCF)
	{
		error("expected CJcf, got %d\n", opcode);
		return False;
	}

	in_uint8(s, result);
	if (result != 0)
	{
		error("CJrq: %d\n", result);
		return False;
	}

	in_uint8s(s, 4);	/* mcs_userid, req_chanid */
	if (opcode & 2)
		in_uint8s(s, 2);	/* join_chanid */

	return s_check_end(s);
}