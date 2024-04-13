is_valid_blob (MonoImage *image, guint32 blob_index, int notnull)
{
	guint32 size;
	const char *p, *blob_end;

	if (blob_index >= image->heap_blob.size)
		return 0;
	p = mono_metadata_blob_heap (image, blob_index);
	size = mono_metadata_decode_blob_size (p, &blob_end);
	if (blob_index + size + (blob_end-p) > image->heap_blob.size)
		return 0;
	if (notnull && !size)
		return 0;
	return 1;
}