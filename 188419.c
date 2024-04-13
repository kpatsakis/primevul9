xps_parse_abbreviated_geometry(xps_document *doc, char *geom, int *fill_rule)
{
	fz_path *path;
	char **args;
	char **pargs;
	char *s = geom;
	fz_point pt;
	int i, n;
	int cmd, old;
	float x1, y1, x2, y2, x3, y3;
	float smooth_x, smooth_y; /* saved cubic bezier control point for smooth curves */
	int reset_smooth;

	path = fz_new_path(doc->ctx);

	args = fz_malloc_array(doc->ctx, strlen(geom) + 1, sizeof(char*));
	pargs = args;

	while (*s)
	{
		if ((*s >= 'A' && *s <= 'Z') || (*s >= 'a' && *s <= 'z'))
		{
			*pargs++ = s++;
		}
		else if ((*s >= '0' && *s <= '9') || *s == '.' || *s == '+' || *s == '-' || *s == 'e' || *s == 'E')
		{
			*pargs++ = s;
			while ((*s >= '0' && *s <= '9') || *s == '.' || *s == '+' || *s == '-' || *s == 'e' || *s == 'E')
				s ++;
		}
		else
		{
			s++;
		}
	}

	*pargs = s;

	n = pargs - args;
	i = 0;

	old = 0;

	reset_smooth = 1;
	smooth_x = 0;
	smooth_y = 0;

	while (i < n)
	{
		cmd = args[i][0];
		if (cmd == '+' || cmd == '.' || cmd == '-' || (cmd >= '0' && cmd <= '9'))
			cmd = old; /* it's a number, repeat old command */
		else
			i ++;

		if (reset_smooth)
		{
			smooth_x = 0;
			smooth_y = 0;
		}

		reset_smooth = 1;

		switch (cmd)
		{
		case 'F':
			if (i >= n) break;
			*fill_rule = atoi(args[i]);
			i ++;
			break;

		case 'M':
			if (i + 1 >= n) break;
			fz_moveto(doc->ctx, path, fz_atof(args[i]), fz_atof(args[i+1]));
			i += 2;
			break;
		case 'm':
			if (i + 1 >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			fz_moveto(doc->ctx, path, pt.x + fz_atof(args[i]), pt.y + fz_atof(args[i+1]));
			i += 2;
			break;

		case 'L':
			if (i + 1 >= n) break;
			fz_lineto(doc->ctx, path, fz_atof(args[i]), fz_atof(args[i+1]));
			i += 2;
			break;
		case 'l':
			if (i + 1 >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			fz_lineto(doc->ctx, path, pt.x + fz_atof(args[i]), pt.y + fz_atof(args[i+1]));
			i += 2;
			break;

		case 'H':
			if (i >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			fz_lineto(doc->ctx, path, fz_atof(args[i]), pt.y);
			i += 1;
			break;
		case 'h':
			if (i >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			fz_lineto(doc->ctx, path, pt.x + fz_atof(args[i]), pt.y);
			i += 1;
			break;

		case 'V':
			if (i >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			fz_lineto(doc->ctx, path, pt.x, fz_atof(args[i]));
			i += 1;
			break;
		case 'v':
			if (i >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			fz_lineto(doc->ctx, path, pt.x, pt.y + fz_atof(args[i]));
			i += 1;
			break;

		case 'C':
			if (i + 5 >= n) break;
			x1 = fz_atof(args[i+0]);
			y1 = fz_atof(args[i+1]);
			x2 = fz_atof(args[i+2]);
			y2 = fz_atof(args[i+3]);
			x3 = fz_atof(args[i+4]);
			y3 = fz_atof(args[i+5]);
			fz_curveto(doc->ctx, path, x1, y1, x2, y2, x3, y3);
			i += 6;
			reset_smooth = 0;
			smooth_x = x3 - x2;
			smooth_y = y3 - y2;
			break;

		case 'c':
			if (i + 5 >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			x1 = fz_atof(args[i+0]) + pt.x;
			y1 = fz_atof(args[i+1]) + pt.y;
			x2 = fz_atof(args[i+2]) + pt.x;
			y2 = fz_atof(args[i+3]) + pt.y;
			x3 = fz_atof(args[i+4]) + pt.x;
			y3 = fz_atof(args[i+5]) + pt.y;
			fz_curveto(doc->ctx, path, x1, y1, x2, y2, x3, y3);
			i += 6;
			reset_smooth = 0;
			smooth_x = x3 - x2;
			smooth_y = y3 - y2;
			break;

		case 'S':
			if (i + 3 >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			x1 = fz_atof(args[i+0]);
			y1 = fz_atof(args[i+1]);
			x2 = fz_atof(args[i+2]);
			y2 = fz_atof(args[i+3]);
			fz_curveto(doc->ctx, path, pt.x + smooth_x, pt.y + smooth_y, x1, y1, x2, y2);
			i += 4;
			reset_smooth = 0;
			smooth_x = x2 - x1;
			smooth_y = y2 - y1;
			break;

		case 's':
			if (i + 3 >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			x1 = fz_atof(args[i+0]) + pt.x;
			y1 = fz_atof(args[i+1]) + pt.y;
			x2 = fz_atof(args[i+2]) + pt.x;
			y2 = fz_atof(args[i+3]) + pt.y;
			fz_curveto(doc->ctx, path, pt.x + smooth_x, pt.y + smooth_y, x1, y1, x2, y2);
			i += 4;
			reset_smooth = 0;
			smooth_x = x2 - x1;
			smooth_y = y2 - y1;
			break;

		case 'Q':
			if (i + 3 >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			x1 = fz_atof(args[i+0]);
			y1 = fz_atof(args[i+1]);
			x2 = fz_atof(args[i+2]);
			y2 = fz_atof(args[i+3]);
			fz_curveto(doc->ctx, path,
				(pt.x + 2 * x1) / 3, (pt.y + 2 * y1) / 3,
				(x2 + 2 * x1) / 3, (y2 + 2 * y1) / 3,
				x2, y2);
			i += 4;
			break;
		case 'q':
			if (i + 3 >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			x1 = fz_atof(args[i+0]) + pt.x;
			y1 = fz_atof(args[i+1]) + pt.y;
			x2 = fz_atof(args[i+2]) + pt.x;
			y2 = fz_atof(args[i+3]) + pt.y;
			fz_curveto(doc->ctx, path,
				(pt.x + 2 * x1) / 3, (pt.y + 2 * y1) / 3,
				(x2 + 2 * x1) / 3, (y2 + 2 * y1) / 3,
				x2, y2);
			i += 4;
			break;

		case 'A':
			if (i + 6 >= n) break;
			xps_draw_arc(doc->ctx, path,
				fz_atof(args[i+0]), fz_atof(args[i+1]), fz_atof(args[i+2]),
				atoi(args[i+3]), atoi(args[i+4]),
				fz_atof(args[i+5]), fz_atof(args[i+6]));
			i += 7;
			break;
		case 'a':
			if (i + 6 >= n) break;
			pt = fz_currentpoint(doc->ctx, path);
			xps_draw_arc(doc->ctx, path,
				fz_atof(args[i+0]), fz_atof(args[i+1]), fz_atof(args[i+2]),
				atoi(args[i+3]), atoi(args[i+4]),
				fz_atof(args[i+5]) + pt.x, fz_atof(args[i+6]) + pt.y);
			i += 7;
			break;

		case 'Z':
		case 'z':
			fz_closepath(doc->ctx, path);
			break;

		default:
			/* eek */
			fz_warn(doc->ctx, "ignoring invalid command '%c'", cmd);
			/* Skip any trailing numbers to avoid an infinite loop */
			while (i < n && (args[i][0] == '+' || args[i][0] == '.' || args[i][0] == '-' || (args[i][0] >= '0' && args[i][0] <= '9')))
				i ++;
			break;
		}

		old = cmd;
	}

	fz_free(doc->ctx, args);
	return path;
}