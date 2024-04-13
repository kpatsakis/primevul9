void Inflator::IsolatedInitialize(const NameValuePairs &parameters)
{
	m_state = PRE_STREAM;
	parameters.GetValue("Repeat", m_repeat);
	m_inQueue.Clear();
	m_reader.SkipBits(m_reader.BitsBuffered());
}