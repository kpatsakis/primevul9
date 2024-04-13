static s32 mpgviddmx_next_start_code(u8 *data, u32 size)
{
	u32 v, bpos, found;
	s64 start, end;

	bpos = 0;
	found = 0;
	start = 0;
	end = 0;

	v = 0xffffffff;
	while (!end) {
		if (bpos == size)
			return -1;
		v = ( (v<<8) & 0xFFFFFF00) | data[bpos];

		bpos++;
		if ((v & 0xFFFFFF00) == 0x00000100) {
			end = start + bpos - 4;
			found = 1;
			break;
		}
	}
	if (!found)
		return -1;
	assert(end >= start);
	return (s32) (end - start);
}