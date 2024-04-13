void Inflator::OutputByte(byte b)
{
	m_window[m_current++] = b;
	if (m_current == m_window.size())
	{
		ProcessDecompressedData(m_window + m_lastFlush, m_window.size() - m_lastFlush);
		m_lastFlush = 0;
		m_current = 0;
		m_wrappedAround = true;
	}
}