process_orders(STREAM s, uint16 num_orders)
{
	RDP_ORDER_STATE *os = &g_order_state;
	uint32 present;
	uint8 order_flags;
	int size, processed = 0;
	RD_BOOL delta;

	while (processed < num_orders)
	{
		in_uint8(s, order_flags);

		if (!(order_flags & RDP_ORDER_STANDARD))
		{
			error("order parsing failed\n");
			break;
		}

		if (order_flags & RDP_ORDER_SECONDARY)
		{
			process_secondary_order(s);
		}
		else
		{
			if (order_flags & RDP_ORDER_CHANGE)
			{
				in_uint8(s, os->order_type);
			}

			switch (os->order_type)
			{
				case RDP_ORDER_TRIBLT:
				case RDP_ORDER_TEXT2:
					size = 3;
					break;

				case RDP_ORDER_PATBLT:
				case RDP_ORDER_MEMBLT:
				case RDP_ORDER_LINE:
				case RDP_ORDER_POLYGON2:
				case RDP_ORDER_ELLIPSE2:
					size = 2;
					break;

				default:
					size = 1;
			}

			rdp_in_present(s, &present, order_flags, size);

			if (order_flags & RDP_ORDER_BOUNDS)
			{
				if (!(order_flags & RDP_ORDER_LASTBOUNDS))
					rdp_parse_bounds(s, &os->bounds);

				ui_set_clip(os->bounds.left,
					    os->bounds.top,
					    os->bounds.right -
					    os->bounds.left + 1,
					    os->bounds.bottom - os->bounds.top + 1);
			}

			delta = order_flags & RDP_ORDER_DELTA;

			switch (os->order_type)
			{
				case RDP_ORDER_DESTBLT:
					process_destblt(s, &os->destblt, present, delta);
					break;

				case RDP_ORDER_PATBLT:
					process_patblt(s, &os->patblt, present, delta);
					break;

				case RDP_ORDER_SCREENBLT:
					process_screenblt(s, &os->screenblt, present, delta);
					break;

				case RDP_ORDER_LINE:
					process_line(s, &os->line, present, delta);
					break;

				case RDP_ORDER_RECT:
					process_rect(s, &os->rect, present, delta);
					break;

				case RDP_ORDER_DESKSAVE:
					process_desksave(s, &os->desksave, present, delta);
					break;

				case RDP_ORDER_MEMBLT:
					process_memblt(s, &os->memblt, present, delta);
					break;

				case RDP_ORDER_TRIBLT:
					process_triblt(s, &os->triblt, present, delta);
					break;

				case RDP_ORDER_POLYGON:
					process_polygon(s, &os->polygon, present, delta);
					break;

				case RDP_ORDER_POLYGON2:
					process_polygon2(s, &os->polygon2, present, delta);
					break;

				case RDP_ORDER_POLYLINE:
					process_polyline(s, &os->polyline, present, delta);
					break;

				case RDP_ORDER_ELLIPSE:
					process_ellipse(s, &os->ellipse, present, delta);
					break;

				case RDP_ORDER_ELLIPSE2:
					process_ellipse2(s, &os->ellipse2, present, delta);
					break;

				case RDP_ORDER_TEXT2:
					process_text2(s, &os->text2, present, delta);
					break;

				default:
					unimpl("order %d\n", os->order_type);
					return;
			}

			if (order_flags & RDP_ORDER_BOUNDS)
				ui_reset_clip();
		}

		processed++;
	}
#if 0
	/* not true when RDP_COMPRESSION is set */
	if (s->p != g_next_packet)
		error("%d bytes remaining\n", (int) (g_next_packet - s->p));
#endif

}