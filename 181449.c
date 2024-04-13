void Inflator::FlushOutput()
{
	if (m_state != PRE_STREAM)
	{
		CRYPTOPP_ASSERT(m_current >= m_lastFlush);
		ProcessDecompressedData(m_window + m_lastFlush, m_current - m_lastFlush);
		m_lastFlush = m_current;
	}
}