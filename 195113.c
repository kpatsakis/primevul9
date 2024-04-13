copy4 (uschar *out, uint32x x)
{
out[0] = x & 0xFF;
out[1] = (x >> 8) & 0xFF;
out[2] = (x >> 16) & 0xFF;
out[3] = (x >> 24) & 0xFF;
}