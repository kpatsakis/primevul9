DoCompressed(struct bmp_progressive_state *context, GError **error)
{
	gint i, j;
	gint y;
	guchar c;
	gint idx;

	/* context->compr.y might be past the last line because we are
	 * on padding past the end of a valid data, or we might have hit
	 * out-of-bounds data. Either way we just eat-and-ignore the
	 * rest of the file. Doing the check only here and not when
	 * we change y below is fine since BufferSize is always 2 here
	 * and the BMP file format always starts new data on 16-bit
	 * boundaries.
	 */
	if (context->compr.y >= context->Header.height) {
		context->BufferDone = 0;
		return TRUE;
	}

	y = context->compr.y;

 	for (i = 0; i < context->BufferSize; i++) {
		c = context->buff[i];
		switch (context->compr.phase) {
		    case NEUTRAL:
			    if (c) {
				    context->compr.run = c;
				    context->compr.phase = ENCODED;
			    }
			    else
				    context->compr.phase = ESCAPE;
			    break;
		    case ENCODED:
			    for (j = 0; j < context->compr.run; j++) {
				    if (context->Compressed == BI_RLE8)
					    idx = c;
				    else if (j & 1) 
					    idx = c & 0x0f;
				    else 
					    idx = (c >> 4) & 0x0f;
				    if (context->compr.x < context->Header.width) {
					    *context->compr.p++ = context->Colormap[idx][2];
					    *context->compr.p++ = context->Colormap[idx][1];
					    *context->compr.p++ = context->Colormap[idx][0];
					    *context->compr.p++ = 0xff;
					    context->compr.x++;    
				    }
			    }
			    context->compr.phase = NEUTRAL;
			    break;
		    case ESCAPE:
			    switch (c) {
				case END_OF_LINE:
					context->compr.x = 0;
					context->compr.y++;
					context->compr.p = context->pixbuf->pixels 
						+ (context->pixbuf->rowstride * (context->Header.height - context->compr.y - 1))
						+ (4 * context->compr.x);
					context->compr.phase = NEUTRAL;
					break;
				case END_OF_BITMAP:
					context->compr.x = 0;
					context->compr.y = context->Header.height;
					context->compr.phase = NEUTRAL;
					break;
				case DELTA:
					context->compr.phase = DELTA_X;
					break;
				default:
					context->compr.run = c;
					context->compr.count = 0;
					context->compr.phase = ABSOLUTE;
					break;
			    }
			    break;
		    case DELTA_X:
			    context->compr.x += c;
			    context->compr.phase = DELTA_Y;
			    break;
		    case DELTA_Y:
			    context->compr.y += c;
			    context->compr.p = context->pixbuf->pixels 
				    + (context->pixbuf->rowstride * (context->Header.height - context->compr.y - 1))
				    + (4 * context->compr.x);
			    context->compr.phase = NEUTRAL;
			    break;
		    case ABSOLUTE:
			    if (context->Compressed == BI_RLE8) {
				    idx = c;
				    if (context->compr.x < context->Header.width) {
					    *context->compr.p++ = context->Colormap[idx][2];
					    *context->compr.p++ = context->Colormap[idx][1];
					    *context->compr.p++ = context->Colormap[idx][0];
					    *context->compr.p++ = 0xff;
					    context->compr.x++;    
				    }
				    context->compr.count++;

				    if (context->compr.count == context->compr.run) {
					    if (context->compr.run & 1)
						    context->compr.phase = SKIP;
					    else
						    context->compr.phase = NEUTRAL;
				    }
			    }
			    else {
				    for (j = 0; j < 2; j++) {
					    if (context->compr.count & 1)
						    idx = c & 0x0f;
					    else 
						    idx = (c >> 4) & 0x0f;
					    if (context->compr.x < context->Header.width) {
						    *context->compr.p++ = context->Colormap[idx][2];
						    *context->compr.p++ = context->Colormap[idx][1];
						    *context->compr.p++ = context->Colormap[idx][0];
						    *context->compr.p++ = 0xff;
						    context->compr.x++;    
					    }
					    context->compr.count++;

					    if (context->compr.count == context->compr.run) {
						    if ((context->compr.run & 3) == 1
							|| (context->compr.run & 3) == 2) 
							    context->compr.phase = SKIP;
						    else
							    context->compr.phase = NEUTRAL;
						    break;
					    }
				    }
			    }
			    break;
		    case SKIP:
			    context->compr.phase = NEUTRAL;
			    break;
		}
	}
	if (context->updated_func != NULL) {
		if (context->compr.y > y)
		{
			gint new_y = MIN (context->compr.y, context->Header.height);
			(*context->updated_func) (context->pixbuf,
						  0,
						  context->Header.height - new_y,
						  context->Header.width,
						  new_y - y,
						  context->user_data);
		}

	}

	context->BufferDone = 0;
	return TRUE;
}