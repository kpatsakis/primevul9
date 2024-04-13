void Inflator::OutputPast(unsigned int length, unsigned int distance)
{
	size_t start;
	if (distance <= m_current)
		start = m_current - distance;
	else if (m_wrappedAround && distance <= m_window.size())
		start = m_current + m_window.size() - distance;
	else
		throw BadBlockErr();

	if (start + length > m_window.size())
	{
		for (; start < m_window.size(); start++, length--)
			OutputByte(m_window[start]);
		start = 0;
	}

	if (start + length > m_current || m_current + length >= m_window.size())
	{
		while (length--)
			OutputByte(m_window[start++]);
	}
	else
	{
		memcpy(m_window + m_current, m_window + start, length);
		m_current += length;
	}
}