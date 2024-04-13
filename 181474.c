inline void LowFirstBitReader::SkipBits(unsigned int length)
{
	CRYPTOPP_ASSERT(m_bitsBuffered >= length);
	m_buffer >>= length;
	m_bitsBuffered -= length;
}