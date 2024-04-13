static void get_map(char *p)
{
	struct map *map;
	struct note_map *note_map;
	char *name, *q;
	int l, type, pit, acc;

	if (*p == '\0')
		return;

	/* map name */
	name = p;
	while (!isspace((unsigned char) *p) && *p != '\0')
		p++;
	l = p - name;

	/* base note */
	while (isspace((unsigned char) *p))
		p++;
	if (*p == '*') {
		type = MAP_ALL;
		p++;
	} else if (strncmp(p, "octave,", 7) == 0) {
		type = MAP_OCT;
		p += 7;
	} else if (strncmp(p, "key,", 4) == 0) {
		type = MAP_KEY;
		p += 4;
	} else if (strncmp(p, "all", 3) == 0) {
		type = MAP_ALL;
		while (!isspace((unsigned char) *p) && *p != '\0')
			p++;
	} else {
		type = MAP_ONE;
	}
	if (type != MAP_ALL) {
		p = parse_acc_pit(p, &pit, &acc);
		if (type == MAP_OCT || type == MAP_KEY) {
			pit %= 7;
			if (type == MAP_KEY)
				acc = A_NULL;
		}
	} else {
		pit = acc = 0;
	}

	// get/create the map
	for (map = maps; map; map = map->next) {
		if (strncmp(name, map->name, l) == 0)
			break;
	}
	if (!map) {
		map = getarena(sizeof *map);
		map->next = maps;
		maps = map;
		map->name = getarena(l + 1);
		strncpy(map->name, name, l);
		map->name[l] = '\0';
		map->notes = NULL;
	}
	for (note_map = map->notes; note_map; note_map = note_map->next) {
		if (note_map->type == type
		 && note_map->pit == pit
		 && note_map->acc == acc)
			break;
	}
	if (!note_map) {
		note_map = getarena(sizeof *note_map);
		memset(note_map, 0, sizeof *note_map);
		note_map->next = map->notes;
		map->notes = note_map;
		note_map->type = type;
		note_map->pit = pit;
		note_map->acc = acc;
		note_map->print_pit = -128;
		note_map->color = -1;
	}

	/* try the optional 'print' and 'heads' parameters */
	while (isspace((unsigned char) *p))
		p++;
	if (*p == '\0')
		return;
	q = p;
	while (!isspace((unsigned char) *q) && *q != '\0') {
		if (*q == '=')
			break;
		q++;
	}
	if (isspace((unsigned char) *q) || *q == '\0') {
		if (*p != '*') {
			p = parse_acc_pit(p, &pit, &acc);
			note_map->print_pit = pit;
			note_map->print_acc = acc;
			if (*p == '\0')
				return;
		}
		p = q;
		while (isspace((unsigned char) *p))
			p++;
		if (*p == '\0')
			return;
		q = p;
		while (!isspace((unsigned char) *q) && *q != '\0') {
			if (*q == '=')
				break;
			q++;
		}
		if (isspace((unsigned char) *q) || *q == '\0') {
			name = p;
			p = q;
			l = p - name;
			note_map->heads = getarena(l + 1);
			strncpy(note_map->heads, name, l);
			note_map->heads[l] = '\0';
		}
	}

	/* loop on the parameters */
	for (;;) {
		while (isspace((unsigned char) *p))
			p++;
		if (*p == '\0')
			break;
		if (strncmp(p, "heads=", 6) == 0) {
			p += 6;
			name = p;
			while (!isspace((unsigned char) *p) && *p != '\0')
				p++;
			l = p - name;
			note_map->heads = getarena(l + 1);
			strncpy(note_map->heads, name, l);
			note_map->heads[l] = '\0';
		} else if (strncmp(p, "print=", 6) == 0) {
			p += 6;
			p = parse_acc_pit(p, &pit, &acc);
			note_map->print_pit = pit;
			note_map->print_acc = acc;
		} else if (strncmp(p, "color=", 6) == 0) {
			int color;

			color = get_color(p + 6);
			if (color < 0) {
				error(1, NULL, "Bad color in %%%%map");
				return;
			}
			note_map->color = color;
		}
		while (!isspace((unsigned char) *p) && *p != '\0')
			p++;
	}
}