static void add_static_types(struct cli_bc *bc)
{
    unsigned i;
    for (i=0;i<NUM_STATIC_TYPES;i++) {
	bc->types[i].kind = DPointerType;
	bc->types[i].numElements = 1;
	bc->types[i].containedTypes = &containedTy[i];
	bc->types[i].size = bc->types[i].align = 8;
    }
}