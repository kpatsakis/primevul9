static void bnx2x_prep_ops(const u8 *_source, u8 *_target, u32 n)
{
	const __be32 *source = (const __be32 *)_source;
	struct raw_op *target = (struct raw_op *)_target;
	u32 i, j, tmp;

	for (i = 0, j = 0; i < n/8; i++, j += 2) {
		tmp = be32_to_cpu(source[j]);
		target[i].op = (tmp >> 24) & 0xff;
		target[i].offset = tmp & 0xffffff;
		target[i].raw_data = be32_to_cpu(source[j + 1]);
	}
}