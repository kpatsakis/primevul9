	HuffmanDecoder(const unsigned int *codeBitLengths, unsigned int nCodes)
		: m_maxCodeBits(0), m_cacheBits(0), m_cacheMask(0), m_normalizedCacheMask(0)
			{Initialize(codeBitLengths, nCodes);}