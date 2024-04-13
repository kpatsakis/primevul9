const HuffmanDecoder& Inflator::GetDistanceDecoder() const
{
	return m_blockType == 1 ? Singleton<HuffmanDecoder, NewFixedDistanceDecoder>().Ref() : m_dynamicDistanceDecoder;
}