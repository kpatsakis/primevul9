static void be16_to_cpu_n(const u8 *_source, u8 *_target, u32 n)
{
	const __be16 *source = (const __be16 *)_source;
	u16 *target = (u16 *)_target;
	u32 i;

	for (i = 0; i < n/2; i++)
		target[i] = be16_to_cpu(source[i]);
}