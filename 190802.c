defaultFormat (Compressor * compressor)
{
    return compressor? compressor->format(): Compressor::XDR;
}