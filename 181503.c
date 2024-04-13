Inflator::Inflator(BufferedTransformation *attachment, bool repeat, int propagation)
	: AutoSignaling<Filter>(propagation)
	, m_state(PRE_STREAM), m_repeat(repeat), m_eof(0), m_wrappedAround(0)
	, m_blockType(0xff), m_storedLen(0xffff), m_nextDecode(), m_literal(0)
	, m_distance(0), m_reader(m_inQueue), m_current(0), m_lastFlush(0)
{
	Detach(attachment);
}