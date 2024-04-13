process_palette(STREAM s)
{
	COLOURENTRY *entry;
	COLOURMAP map;
	RD_HCOLOURMAP hmap;
	int i;

	in_uint8s(s, 2);	/* pad */
	in_uint16_le(s, map.ncolours);
	in_uint8s(s, 2);	/* pad */

	map.colours = (COLOURENTRY *) xmalloc(sizeof(COLOURENTRY) * map.ncolours);

	DEBUG(("PALETTE(c=%d)\n", map.ncolours));

	for (i = 0; i < map.ncolours; i++)
	{
		entry = &map.colours[i];
		in_uint8(s, entry->red);
		in_uint8(s, entry->green);
		in_uint8(s, entry->blue);
	}

	hmap = ui_create_colourmap(&map);
	ui_set_colourmap(hmap);

	xfree(map.colours);
}