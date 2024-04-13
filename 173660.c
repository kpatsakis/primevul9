static int get_color(char *p)
{
	int i, color;
	static const struct {
		char *name;
		int color;
	} col_tb[] = {
		{ "aqua",	0x00ffff },
		{ "black",	0x000000 },
		{ "blue",	0x0000ff },
		{ "fuchsia",	0xff00ff },
		{ "gray",	0x808080 },
		{ "green",	0x008000 },
		{ "lime",	0x00ff00 },
		{ "maroon",	0x800000 },
		{ "navy",	0x000080 },
		{ "olive",	0x808000 },
		{ "purple",	0x800080 },
		{ "red",	0xff0000 },
		{ "silver",	0xc0c0c0 },
		{ "teal",	0x008080 },
		{ "white",	0xffffff },
		{ "yellow",	0xffff00 },
	};

	if (*p == '#') {
		if (sscanf(p, "#%06x", &color) != 1
		 || (unsigned) color > 0x00ffffff)
			return -1;
		return color;
	}
	for (i = sizeof col_tb / sizeof col_tb[0]; --i >= 0; ) {
		if (strncasecmp(p, col_tb[i].name,
				strlen(col_tb[i].name)) == 0)
			break;
	}
	if (i < 0)
		return -1;
	return col_tb[i].color;
}