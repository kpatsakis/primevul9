static inline void decode_eat32bits(Encoder *encoder)
{
    decode_eatbits(encoder, 16);
    decode_eatbits(encoder, 16);
}