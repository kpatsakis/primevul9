cms_context_init(cms_context *cms)
{
	memset(cms, '\0', sizeof (*cms));

	cms->log = cms_common_log;

	cms->arena = PORT_NewArena(DER_DEFAULT_CHUNKSIZE);
	if (!cms->arena)
		cmsreterr(-1, cms, "could not create cryptographic arena");

	cms->selected_digest = -1;

	return 0;
}