void rfx_context_set_pixel_format(RFX_CONTEXT* context, UINT32 pixel_format)
{
	context->pixel_format = pixel_format;
	context->bits_per_pixel = GetBitsPerPixel(pixel_format);
}