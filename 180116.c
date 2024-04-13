mono_metadata_memberref_is_method (MonoImage *image, guint32 token)
{
	guint32 cols [MONO_MEMBERREF_SIZE];
	const char *sig;
	mono_metadata_decode_row (&image->tables [MONO_TABLE_MEMBERREF], mono_metadata_token_index (token) - 1, cols, MONO_MEMBERREF_SIZE);
	sig = mono_metadata_blob_heap (image, cols [MONO_MEMBERREF_SIGNATURE]);
	mono_metadata_decode_blob_size (sig, &sig);
	return (*sig != 0x6);
}