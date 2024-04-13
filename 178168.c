int LUKS2_hdr_dump(struct crypt_device *cd, struct luks2_hdr *hdr)
{
	if (!hdr->jobj)
		return -EINVAL;

	JSON_DBG(cd, hdr->jobj, NULL);

	log_std(cd, "LUKS header information\n");
	log_std(cd, "Version:       \t%u\n", hdr->version);
	log_std(cd, "Epoch:         \t%" PRIu64 "\n", hdr->seqid);
	log_std(cd, "Metadata area: \t%" PRIu64 " [bytes]\n", LUKS2_metadata_size(hdr));
	log_std(cd, "Keyslots area: \t%" PRIu64 " [bytes]\n", LUKS2_keyslots_size(hdr));
	log_std(cd, "UUID:          \t%s\n", *hdr->uuid ? hdr->uuid : "(no UUID)");
	log_std(cd, "Label:         \t%s\n", *hdr->label ? hdr->label : "(no label)");
	log_std(cd, "Subsystem:     \t%s\n", *hdr->subsystem ? hdr->subsystem : "(no subsystem)");

	hdr_dump_config(cd, hdr->jobj);
	hdr_dump_segments(cd, hdr->jobj);
	hdr_dump_keyslots(cd, hdr->jobj);
	hdr_dump_tokens(cd, hdr->jobj);
	hdr_dump_digests(cd, hdr->jobj);

	return 0;
}