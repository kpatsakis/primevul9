bool HuffmanDecoder::Decode(LowFirstBitReader &reader, value_t &value) const
{
	bool result = reader.FillBuffer(m_maxCodeBits);
	CRYPTOPP_UNUSED(result); // CRYPTOPP_ASSERT(result);

	unsigned int codeBits = Decode(reader.PeekBuffer(), value);
	if (codeBits > reader.BitsBuffered())
		return false;
	reader.SkipBits(codeBits);
	return true;
}