process_desksave(STREAM s, DESKSAVE_ORDER * os, uint32 present, RD_BOOL delta)
{
	int width, height;

	if (present & 0x01)
		in_uint32_le(s, os->offset);

	if (present & 0x02)
		rdp_in_coord(s, &os->left, delta);

	if (present & 0x04)
		rdp_in_coord(s, &os->top, delta);

	if (present & 0x08)
		rdp_in_coord(s, &os->right, delta);

	if (present & 0x10)
		rdp_in_coord(s, &os->bottom, delta);

	if (present & 0x20)
		in_uint8(s, os->action);

	DEBUG(("DESKSAVE(l=%d,t=%d,r=%d,b=%d,off=%d,op=%d)\n",
	       os->left, os->top, os->right, os->bottom, os->offset, os->action));

	width = os->right - os->left + 1;
	height = os->bottom - os->top + 1;

	if (os->action == 0)
		ui_desktop_save(os->offset, os->left, os->top, width, height);
	else
		ui_desktop_restore(os->offset, os->left, os->top, width, height);
}