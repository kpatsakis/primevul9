static void OneLine(struct bmp_progressive_state *context)
{
	context->BufferDone = 0;
	if (context->Lines >= context->Header.height)
		return;

	if (context->Type == 32)
		OneLine32(context);
	else if (context->Type == 24)
		OneLine24(context);
	else if (context->Type == 16)
		OneLine16(context);
	else if (context->Type == 8)
		OneLine8(context);
	else if (context->Type == 4)
		OneLine4(context);
	else if (context->Type == 1)
		OneLine1(context);
	else
		g_assert_not_reached ();

	context->Lines++;

	if (context->updated_func != NULL) {
		(*context->updated_func) (context->pixbuf,
					  0,
					  (context->Header.Negative ?
					   (context->Lines - 1) :
					   (context->Header.height - context->Lines)),
					  context->Header.width,
					  1,
					  context->user_data);

	}
}