gdk_pixbuf__bmp_image_load_increment(gpointer data,
                                     const guchar * buf,
                                     guint size,
                                     GError **error)
{
	struct bmp_progressive_state *context =
	    (struct bmp_progressive_state *) data;

	gint BytesToCopy;
	gint BytesToRemove;

	if (context->read_state == READ_STATE_DONE)
		return TRUE;
	else if (context->read_state == READ_STATE_ERROR)
		return FALSE;

	while (size > 0) {
		if (context->BufferDone < context->BufferSize) {	/* We still
									   have headerbytes to do */
			BytesToCopy =
			    context->BufferSize - context->BufferDone;
			if (BytesToCopy > size)
				BytesToCopy = size;

			memmove(context->buff + context->BufferDone,
				buf, BytesToCopy);

			size -= BytesToCopy;
			buf += BytesToCopy;
			context->BufferDone += BytesToCopy;

			if (context->BufferDone != context->BufferSize)
				break;
		}

		/* context->buff is full.  Now we discard all "padding" */
		if (context->BufferPadding != 0) {
			BytesToRemove = context->BufferPadding - size;
			if (BytesToRemove > size) {
				BytesToRemove = size;
			}
			size -= BytesToRemove;
			context->BufferPadding -= BytesToRemove;

			if (context->BufferPadding != 0)
				break;
		}

		switch (context->read_state) {
		case READ_STATE_HEADERS:
			if (!DecodeHeader (context->buff,
					   context->buff + 14, context,
					   error))
				return FALSE;

			break;

		case READ_STATE_PALETTE:
			if (!DecodeColormap (context->buff, context, error))
				return FALSE;
			break;

		case READ_STATE_BITMASKS:
			if (!decode_bitmasks (context->buff, context, error))
				return FALSE;
			break;

		case READ_STATE_DATA:
			if (context->Compressed == BI_RGB || context->Compressed == BI_BITFIELDS)
				OneLine (context);
			else if (!DoCompressed (context, error))
				return FALSE;

			break;
		case READ_STATE_DONE:
			return TRUE;
			break;

		default:
			g_assert_not_reached ();
		}
	}

	return TRUE;
}