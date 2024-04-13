static inline void encode_32(Encoder *encoder, unsigned int word)
{
    encode(encoder, word >> 16, 16);
    encode(encoder, word & 0x0000ffff, 16);
}