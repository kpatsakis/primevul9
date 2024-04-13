mcs_recv_aucf(uint16 * mcs_userid)
{
	uint8 opcode, result;
	STREAM s;

	s = iso_recv(NULL);
	if (s == NULL)
		return False;

	in_uint8(s, opcode);
	if ((opcode >> 2) != MCS_AUCF)
	{
		error("expected AUcf, got %d\n", opcode);
		return False;
	}

	in_uint8(s, result);
	if (result != 0)
	{
		error("AUrq: %d\n", result);
		return False;
	}

	if (opcode & 2)
		in_uint16_be(s, *mcs_userid);

	return s_check_end(s);
}