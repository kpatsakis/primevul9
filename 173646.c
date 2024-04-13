static void set_map(struct SYMBOL *s)
{
	struct map *map;
	struct note_map *note_map;
	struct note *note;
	int m, delta;

	for (map = maps; map; map = map->next) {
		if (strcmp(map->name, curvoice->map_name) == 0)
			break;
	}
	if (!map)
		return;			// !?

	// loop on the note maps, then on the notes of the chord
	delta = curvoice->ckey.key_delta;
	for (m = 0; m <= s->nhd; m++) {
		note = &s->u.note.notes[m];
		for (note_map = map->notes; note_map; note_map = note_map->next) {
			switch (note_map->type) {
			case MAP_ONE:
				if (note->pit == note_map->pit
				 && note->acc == note_map->acc)
					break;
				continue;
			case MAP_OCT:
				if ((note->pit - note_map->pit + 28 ) % 7 == 0
				 && note->acc == note_map->acc)
					break;
				continue;
			case MAP_KEY:
				if ((note->pit + 28 - delta - note_map->pit) % 7 == 0)
					break;
				continue;
			default: // MAP_ALL
				break;
			}
			note->head = note_map->heads;
			note->color = note_map->color;
			if (note_map->print_pit != -128) {
				note->pit = note_map->print_pit;
				s->pits[m] = note->pit;
				note->acc = note_map->print_acc;
			}
			break;
		}
	}
}