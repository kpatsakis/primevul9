static void save_maps(void)
{
	struct map *omap, *map;
	struct note_map *onotes, *notes;

	omap = maps;
	if (!omap) {
		maps_glob = NULL;
		return;
	}
	maps_glob = map = getarena(sizeof *maps_glob);
	for (;;) {
		memcpy(map, omap, sizeof *map);
		onotes = omap->notes;
		if (onotes) {
			map->notes = notes = getarena(sizeof *notes);
			for (;;) {
				memcpy(notes, onotes, sizeof *notes);
				onotes = onotes->next;
				if (!onotes)
					break;
				notes->next = getarena(sizeof *notes);
				notes = notes->next;
			}
		}
		omap = omap->next;
		if (!omap)
			break;
		map->next = getarena(sizeof *map);
		map = map->next;
	}
}