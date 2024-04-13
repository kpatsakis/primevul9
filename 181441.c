void HuffmanDecoder::FillCacheEntry(LookupEntry &entry, code_t normalizedCode) const
{
	normalizedCode &= m_normalizedCacheMask;
	const CodeInfo &codeInfo = *(std::upper_bound(m_codeToValue.begin(), m_codeToValue.end(), normalizedCode, CodeLessThan())-1);
	if (codeInfo.len <= m_cacheBits)
	{
		entry.type = 1;
		entry.value = codeInfo.value;
		entry.len = codeInfo.len;
	}
	else
	{
		entry.begin = &codeInfo;
		const CodeInfo *last = & *(std::upper_bound(m_codeToValue.begin(), m_codeToValue.end(), normalizedCode + ~m_normalizedCacheMask, CodeLessThan())-1);
		if (codeInfo.len == last->len)
		{
			entry.type = 2;
			entry.len = codeInfo.len;
		}
		else
		{
			entry.type = 3;
			entry.end = last+1;
		}
	}
}