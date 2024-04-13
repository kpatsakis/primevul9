const HuffmanDecoder& Inflator::GetLiteralDecoder() const
{
	return m_blockType == 1 ? Singleton<HuffmanDecoder, NewFixedLiteralDecoder>().Ref() : m_dynamicLiteralDecoder;
}