int LUKS2_get_keyslot_stored_key_size(struct luks2_hdr *hdr, int keyslot)
{
	char keyslot_name[16];

	if (snprintf(keyslot_name, sizeof(keyslot_name), "%u", keyslot) < 1)
		return -1;

	return LUKS2_keyslot_get_volume_key_size(hdr, keyslot_name);
}