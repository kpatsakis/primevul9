inline bool LowFirstBitReader::FillBuffer(unsigned int length)
{
	while (m_bitsBuffered < length)
	{
		byte b;
		if (!m_store.Get(b))
			return false;
		m_buffer |= (unsigned long)b << m_bitsBuffered;
		m_bitsBuffered += 8;
	}
	CRYPTOPP_ASSERT(m_bitsBuffered <= sizeof(unsigned long)*8);
	return true;
}