lshift_a (uint32x x, int s)
{
x &= 0xFFFFFFFF;
return ((x << s) & 0xFFFFFFFF) | (x >> (32 - s));
}