void Inflator::DecodeHeader()
{
	if (!m_reader.FillBuffer(3))
		throw UnexpectedEndErr();
	m_eof = m_reader.GetBits(1) != 0;
	m_blockType = (byte)m_reader.GetBits(2);
	switch (m_blockType)
	{
	case 0:	// stored
		{
		m_reader.SkipBits(m_reader.BitsBuffered() % 8);
		if (!m_reader.FillBuffer(32))
			throw UnexpectedEndErr();
		m_storedLen = (word16)m_reader.GetBits(16);
		word16 nlen = (word16)m_reader.GetBits(16);
		if (nlen != (word16)~m_storedLen)
			throw BadBlockErr();
		break;
		}
	case 1:	// fixed codes
		m_nextDecode = LITERAL;
		break;
	case 2:	// dynamic codes
		{
		if (!m_reader.FillBuffer(5+5+4))
			throw UnexpectedEndErr();
		unsigned int hlit = m_reader.GetBits(5);
		unsigned int hdist = m_reader.GetBits(5);
		unsigned int hclen = m_reader.GetBits(4);

		FixedSizeSecBlock<unsigned int, 286+32> codeLengths;
		unsigned int i;
		static const unsigned int border[] = {    // Order of the bit length code lengths
			16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
		std::fill(codeLengths.begin(), codeLengths+19, 0);
		for (i=0; i<hclen+4; i++)
			codeLengths[border[i]] = m_reader.GetBits(3);

		try
		{
			HuffmanDecoder codeLengthDecoder(codeLengths, 19);
			for (i = 0; i < hlit+257+hdist+1; )
			{
				unsigned int k = 0, count = 0, repeater = 0;
				bool result = codeLengthDecoder.Decode(m_reader, k);
				if (!result)
					throw UnexpectedEndErr();
				if (k <= 15)
				{
					count = 1;
					repeater = k;
				}
				else switch (k)
				{
				case 16:
					if (!m_reader.FillBuffer(2))
						throw UnexpectedEndErr();
					count = 3 + m_reader.GetBits(2);
					if (i == 0)
						throw BadBlockErr();
					repeater = codeLengths[i-1];
					break;
				case 17:
					if (!m_reader.FillBuffer(3))
						throw UnexpectedEndErr();
					count = 3 + m_reader.GetBits(3);
					repeater = 0;
					break;
				case 18:
					if (!m_reader.FillBuffer(7))
						throw UnexpectedEndErr();
					count = 11 + m_reader.GetBits(7);
					repeater = 0;
					break;
				}
				if (i + count > hlit+257+hdist+1)
					throw BadBlockErr();
				std::fill(codeLengths + i, codeLengths + i + count, repeater);
				i += count;
			}
			m_dynamicLiteralDecoder.Initialize(codeLengths, hlit+257);
			if (hdist == 0 && codeLengths[hlit+257] == 0)
			{
				if (hlit != 0)	// a single zero distance code length means all literals
					throw BadBlockErr();
			}
			else
				m_dynamicDistanceDecoder.Initialize(codeLengths+hlit+257, hdist+1);
			m_nextDecode = LITERAL;
		}
		catch (HuffmanDecoder::Err &)
		{
			throw BadBlockErr();
		}
		break;
		}
	default:
		throw BadBlockErr();	// reserved block type
	}
	m_state = DECODING_BODY;
}