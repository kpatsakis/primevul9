static unsigned typealign(const struct cli_bc *bc, uint16_t type)
{
    type &= 0x7fff;
    if (type <= 64) {
	unsigned size = typesize(bc, type);
	return size ? size : 1;
    }
    return bc->types[type-65].align;
}