rdp_parse_bounds(STREAM s, BOUNDS * bounds)
{
	uint8 present;

	in_uint8(s, present);

	if (present & 1)
		rdp_in_coord(s, &bounds->left, False);
	else if (present & 16)
		rdp_in_coord(s, &bounds->left, True);

	if (present & 2)
		rdp_in_coord(s, &bounds->top, False);
	else if (present & 32)
		rdp_in_coord(s, &bounds->top, True);

	if (present & 4)
		rdp_in_coord(s, &bounds->right, False);
	else if (present & 64)
		rdp_in_coord(s, &bounds->right, True);

	if (present & 8)
		rdp_in_coord(s, &bounds->bottom, False);
	else if (present & 128)
		rdp_in_coord(s, &bounds->bottom, True);

	return s_check(s);
}