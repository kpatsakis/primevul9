decode_bitmasks (guchar *buf,
		 struct bmp_progressive_state *State, 
		 GError **error)
{
        State->a_mask = State->a_shift = State->a_bits = 0;
	State->r_mask = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	buf += 4;

	State->g_mask = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	buf += 4;

	State->b_mask = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

	find_bits (State->r_mask, &State->r_shift, &State->r_bits);
	find_bits (State->g_mask, &State->g_shift, &State->g_bits);
	find_bits (State->b_mask, &State->b_shift, &State->b_bits);

        /* v4 and v5 have an alpha mask */
        if (State->Header.size == 108 || State->Header.size == 124) {
	      buf += 4;
	      State->a_mask = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	      find_bits (State->a_mask, &State->a_shift, &State->a_bits);
        }

	if (State->r_bits == 0 || State->g_bits == 0 || State->b_bits == 0) {
                if (State->Type == 16) {
		       State->r_mask = 0x7c00;
		       State->r_shift = 10;
		       State->g_mask = 0x03e0;
		       State->g_shift = 5;
		       State->b_mask = 0x001f;
		       State->b_shift = 0;

		       State->r_bits = State->g_bits = State->b_bits = 5;
                }
                else {
		       State->r_mask = 0x00ff0000;
		       State->r_shift = 16;
		       State->g_mask = 0x0000ff00;
		       State->g_shift = 8;
		       State->b_mask = 0x000000ff;
		       State->b_shift = 0;
		       State->a_mask = 0xff000000;
		       State->a_shift = 24;

		       State->r_bits = State->g_bits = State->b_bits = State->a_bits = 8;
                }
	}

        if (State->r_bits > 8) {
          State->r_shift += State->r_bits - 8;
          State->r_bits = 8;
        }
        if (State->g_bits > 8) {
          State->g_shift += State->g_bits - 8;
          State->g_bits = 8;
        }
        if (State->b_bits > 8) {
          State->b_shift += State->b_bits - 8;
          State->b_bits = 8;
        }
        if (State->a_bits > 8) {
          State->a_shift += State->a_bits - 8;
          State->a_bits = 8;
        }

	State->read_state = READ_STATE_DATA;
	State->BufferDone = 0;
	State->BufferSize = State->LineWidth;
	if (!grow_buffer (State, error)) 
		return FALSE;

	return TRUE;
}