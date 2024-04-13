inline HuffmanDecoder::code_t HuffmanDecoder::NormalizeCode(HuffmanDecoder::code_t code, unsigned int codeBits)
{
	return code << (MAX_CODE_BITS - codeBits);
}