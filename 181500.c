size_t Inflator::Put2(const byte *inString, size_t length, int messageEnd, bool blocking)
{
	if (!blocking)
		throw BlockingInputOnly("Inflator");

	LazyPutter lp(m_inQueue, inString, length);
	ProcessInput(messageEnd != 0);

	if (messageEnd)
		if (!(m_state == PRE_STREAM || m_state == AFTER_END))
			throw UnexpectedEndErr();

	Output(0, NULLPTR, 0, messageEnd, blocking);
	return 0;
}