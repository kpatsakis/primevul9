inline unsigned long LowFirstBitReader::GetBits(unsigned int length)
{
	unsigned long result = PeekBits(length);
	SkipBits(length);
	return result;
}