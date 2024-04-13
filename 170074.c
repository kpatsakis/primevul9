static gboolean DecodeColormap (guchar *buff,
				struct bmp_progressive_state *State,
				GError **error)
{
	gint i;
	gint samples;

	g_assert (State->read_state == READ_STATE_PALETTE);

	samples = (State->Header.size == 12 ? 3 : 4);
	if (State->BufferSize < State->Header.n_colors * samples) {
		State->BufferSize = State->Header.n_colors * samples;
		if (!grow_buffer (State, error))
			return FALSE;
		return TRUE;
	}

	State->Colormap = g_malloc0 ((1 << State->Header.depth) * sizeof (*State->Colormap));
	for (i = 0; i < State->Header.n_colors; i++)

	{
		State->Colormap[i][0] = buff[i * samples];
		State->Colormap[i][1] = buff[i * samples + 1];
		State->Colormap[i][2] = buff[i * samples + 2];
#ifdef DUMPCMAP
		g_print ("color %d %x %x %x\n", i,
			 State->Colormap[i][0],
			 State->Colormap[i][1],
			 State->Colormap[i][2]);
#endif
	}

	State->read_state = READ_STATE_DATA;

	State->BufferDone = 0;
	if (!(State->Compressed == BI_RGB || State->Compressed == BI_BITFIELDS))
		State->BufferSize = 2;
	else
		State->BufferSize = State->LineWidth;
	
	if (!grow_buffer (State, error))
		return FALSE;

	return TRUE;
}