static void llsec_geniv(u8 iv[16], __le64 addr,
			const struct ieee802154_sechdr *sec)
{
	__be64 addr_bytes = (__force __be64) swab64((__force u64) addr);
	__be32 frame_counter = (__force __be32) swab32((__force u32) sec->frame_counter);

	iv[0] = 1; /* L' = L - 1 = 1 */
	memcpy(iv + 1, &addr_bytes, sizeof(addr_bytes));
	memcpy(iv + 9, &frame_counter, sizeof(frame_counter));
	iv[13] = sec->level;
	iv[14] = 0;
	iv[15] = 1;
}