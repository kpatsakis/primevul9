inline unsigned int HuffmanDecoder::Decode(code_t code, /* out */ value_t &value) const
{
	CRYPTOPP_ASSERT(m_codeToValue.size() > 0);
	LookupEntry &entry = m_cache[code & m_cacheMask];

	code_t normalizedCode = 0;
	if (entry.type != 1)
		normalizedCode = BitReverse(code);

	if (entry.type == 0)
		FillCacheEntry(entry, normalizedCode);

	if (entry.type == 1)
	{
		value = entry.value;
		return entry.len;
	}
	else
	{
		const CodeInfo &codeInfo = (entry.type == 2)
			? entry.begin[(normalizedCode << m_cacheBits) >> (MAX_CODE_BITS - (entry.len - m_cacheBits))]
			: *(std::upper_bound(entry.begin, entry.end, normalizedCode, CodeLessThan())-1);
		value = codeInfo.value;
		return codeInfo.len;
	}
}