void Inflator::ProcessInput(bool flush)
{
	while (true)
	{
		switch (m_state)
		{
		case PRE_STREAM:
			if (!flush && m_inQueue.CurrentSize() < MaxPrestreamHeaderSize())
				return;
			ProcessPrestreamHeader();
			m_state = WAIT_HEADER;
			m_wrappedAround = false;
			m_current = 0;
			m_lastFlush = 0;
			m_window.New(((size_t) 1) << GetLog2WindowSize());
			break;
		case WAIT_HEADER:
			{
			// maximum number of bytes before actual compressed data starts
			const size_t MAX_HEADER_SIZE = BitsToBytes(3+5+5+4+19*7+286*15+19*15);
			if (m_inQueue.CurrentSize() < (flush ? 1 : MAX_HEADER_SIZE))
				return;
			DecodeHeader();
			break;
			}
		case DECODING_BODY:
			if (!DecodeBody())
				return;
			break;
		case POST_STREAM:
			if (!flush && m_inQueue.CurrentSize() < MaxPoststreamTailSize())
				return;
			ProcessPoststreamTail();
			m_state = m_repeat ? PRE_STREAM : AFTER_END;
			Output(0, NULLPTR, 0, GetAutoSignalPropagation(), true);	// TODO: non-blocking
			if (m_inQueue.IsEmpty())
				return;
			break;
		case AFTER_END:
			m_inQueue.TransferTo(*AttachedTransformation());
			return;
		}
	}
}