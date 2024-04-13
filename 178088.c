void LUKS2_hdr_free(struct crypt_device *cd, struct luks2_hdr *hdr)
{
	if (json_object_put(hdr->jobj))
		hdr->jobj = NULL;
	else if (hdr->jobj)
		log_dbg(cd, "LUKS2 header still in use");
}