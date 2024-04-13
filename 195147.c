G (uint32x X, uint32x Y, uint32x Z)
{
return (X & Y) | (X & Z) | (Y & Z);
}