verify_standalonesig_table (MonoImage *image, GSList *list, int level)
{
	MonoTableInfo *t = &image->tables [MONO_TABLE_STANDALONESIG];
	guint32 cols [MONO_STAND_ALONE_SIGNATURE_SIZE];
	const char *p;
	guint32 i;

	for (i = 0; i < t->rows; ++i) {
		mono_metadata_decode_row (t, i, cols, MONO_STAND_ALONE_SIGNATURE_SIZE);
		if (level & MONO_VERIFY_ERROR) {
			if (!is_valid_blob (image, cols [MONO_STAND_ALONE_SIGNATURE], TRUE)) {
				ADD_ERROR (list, g_strdup_printf ("Signature is invalid in StandAloneSig row %d", i + 1));
			} else {
				p = mono_metadata_blob_heap (image, cols [MONO_STAND_ALONE_SIGNATURE]);
				/* FIXME: check it's a valid locals or method sig.*/
			}
		}
	}
	return list;
}