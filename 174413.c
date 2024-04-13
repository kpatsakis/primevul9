process_bitmap_updates(STREAM s)
{
	uint16 num_updates;
	int i;

	in_uint16_le(s, num_updates);	/* rectangles */

	for (i = 0; i < num_updates; i++)
	{
		process_bitmap_data(s);
	}
}