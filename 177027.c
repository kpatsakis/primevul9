static int close_blocks(u64 blocknr, u64 other, u32 blocksize)
{
	if (blocknr < other && other - (blocknr + blocksize) < 32768)
		return 1;
	if (blocknr > other && blocknr - (other + blocksize) < 32768)
		return 1;
	return 0;
}