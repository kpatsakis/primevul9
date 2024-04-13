static void bnx2x_prep_iro(const u8 *_source, u8 *_target, u32 n)
{
	const __be32 *source = (const __be32 *)_source;
	struct iro *target = (struct iro *)_target;
	u32 i, j, tmp;

	for (i = 0, j = 0; i < n/sizeof(struct iro); i++) {
		target[i].base = be32_to_cpu(source[j]);
		j++;
		tmp = be32_to_cpu(source[j]);
		target[i].m1 = (tmp >> 16) & 0xffff;
		target[i].m2 = tmp & 0xffff;
		j++;
		tmp = be32_to_cpu(source[j]);
		target[i].m3 = (tmp >> 16) & 0xffff;
		target[i].size = tmp & 0xffff;
		j++;
	}
}