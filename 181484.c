inline unsigned long LowFirstBitReader::PeekBits(unsigned int length)
{
	bool result = FillBuffer(length);
	CRYPTOPP_UNUSED(result); CRYPTOPP_ASSERT(result);
	return m_buffer & (((unsigned long)1 << length) - 1);
}