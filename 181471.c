void HuffmanDecoder::Initialize(const unsigned int *codeBits, unsigned int nCodes)
{
	// the Huffman codes are represented in 3 ways in this code:
	//
	// 1. most significant code bit (i.e. top of code tree) in the least significant bit position
	// 2. most significant code bit (i.e. top of code tree) in the most significant bit position
	// 3. most significant code bit (i.e. top of code tree) in n-th least significant bit position,
	//    where n is the maximum code length for this code tree
	//
	// (1) is the way the codes come in from the deflate stream
	// (2) is used to sort codes so they can be binary searched
	// (3) is used in this function to compute codes from code lengths
	//
	// a code in representation (2) is called "normalized" here
	// The BitReverse() function is used to convert between (1) and (2)
	// The NormalizeCode() function is used to convert from (3) to (2)

	if (nCodes == 0)
		throw Err("null code");

	m_maxCodeBits = *std::max_element(codeBits, codeBits+nCodes);

	if (m_maxCodeBits > MAX_CODE_BITS)
		throw Err("code length exceeds maximum");

	if (m_maxCodeBits == 0)
		throw Err("null code");

	// count number of codes of each length
	SecBlockWithHint<unsigned int, 15+1> blCount(m_maxCodeBits+1);
	std::fill(blCount.begin(), blCount.end(), 0);
	unsigned int i;
	for (i=0; i<nCodes; i++)
		blCount[codeBits[i]]++;

	// compute the starting code of each length
	code_t code = 0;
	SecBlockWithHint<code_t, 15+1> nextCode(m_maxCodeBits+1);
	nextCode[1] = 0;
	for (i=2; i<=m_maxCodeBits; i++)
	{
		// compute this while checking for overflow: code = (code + blCount[i-1]) << 1
		if (code > code + blCount[i-1])
			throw Err("codes oversubscribed");
		code += blCount[i-1];
		if (code > (code << 1))
			throw Err("codes oversubscribed");
		code <<= 1;
		nextCode[i] = code;
	}

	// MAX_CODE_BITS is 32, m_maxCodeBits may be smaller.
	const word64 shiftedMaxCode = ((word64)1 << m_maxCodeBits);
	if (code > shiftedMaxCode - blCount[m_maxCodeBits])
		throw Err("codes oversubscribed");
	else if (m_maxCodeBits != 1 && code < shiftedMaxCode - blCount[m_maxCodeBits])
		throw Err("codes incomplete");

	// compute a vector of <code, length, value> triples sorted by code
	m_codeToValue.resize(nCodes - blCount[0]);
	unsigned int j=0;
	for (i=0; i<nCodes; i++)
	{
		unsigned int len = codeBits[i];
		if (len != 0)
		{
			code = NormalizeCode(nextCode[len]++, len);
			m_codeToValue[j].code = code;
			m_codeToValue[j].len = len;
			m_codeToValue[j].value = i;
			j++;
		}
	}
	std::sort(m_codeToValue.begin(), m_codeToValue.end());

	// initialize the decoding cache
	m_cacheBits = STDMIN(9U, m_maxCodeBits);
	m_cacheMask = (1 << m_cacheBits) - 1;
	m_normalizedCacheMask = NormalizeCode(m_cacheMask, m_cacheBits);
	CRYPTOPP_ASSERT(m_normalizedCacheMask == BitReverse(m_cacheMask));

	const word64 shiftedCache = ((word64)1 << m_cacheBits);
	CRYPTOPP_ASSERT(shiftedCache <= SIZE_MAX);
	if (m_cache.size() != shiftedCache)
		m_cache.resize((size_t)shiftedCache);

	for (i=0; i<m_cache.size(); i++)
		m_cache[i].type = 0;
}