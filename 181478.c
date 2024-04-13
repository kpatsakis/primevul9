void Inflator::OutputString(const byte *string, size_t length)
{
	while (length)
	{
		size_t len = UnsignedMin(length, m_window.size() - m_current);
		memcpy(m_window + m_current, string, len);
		m_current += len;
		if (m_current == m_window.size())
		{
			ProcessDecompressedData(m_window + m_lastFlush, m_window.size() - m_lastFlush);
			m_lastFlush = 0;
			m_current = 0;
			m_wrappedAround = true;
		}
		string += len;
		length -= len;
	}
}