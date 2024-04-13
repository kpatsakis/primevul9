BITMAP_PLANAR_CONTEXT* freerdp_bitmap_planar_context_new(DWORD flags, UINT32 maxWidth,
                                                         UINT32 maxHeight)
{
	BITMAP_PLANAR_CONTEXT* context;
	context = (BITMAP_PLANAR_CONTEXT*)calloc(1, sizeof(BITMAP_PLANAR_CONTEXT));

	if (!context)
		return NULL;

	if (flags & PLANAR_FORMAT_HEADER_NA)
		context->AllowSkipAlpha = TRUE;

	if (flags & PLANAR_FORMAT_HEADER_RLE)
		context->AllowRunLengthEncoding = TRUE;

	if (flags & PLANAR_FORMAT_HEADER_CS)
		context->AllowColorSubsampling = TRUE;

	context->ColorLossLevel = flags & PLANAR_FORMAT_HEADER_CLL_MASK;

	if (context->ColorLossLevel)
		context->AllowDynamicColorFidelity = TRUE;

	if (!freerdp_bitmap_planar_context_reset(context, maxWidth, maxHeight))
	{
		freerdp_bitmap_planar_context_free(context);
		return NULL;
	}

	return context;
}