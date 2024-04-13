GSList*
mono_image_verify_tables (MonoImage *image, int level)
{
	/* The verifier was disabled at compile time */
	return NULL;