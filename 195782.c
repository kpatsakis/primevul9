static int types_equal(const struct cli_bc *bc, uint16_t *apity2ty, uint16_t tid, uint16_t apitid)
{
    unsigned i;
    const struct cli_bc_type *ty = &bc->types[tid - 65];
    const struct cli_bc_type *apity = &cli_apicall_types[apitid];
    /* If we've already verified type equality, return.
     * Since we need to check equality of recursive types, we assume types are
     * equal while checking equality of contained types, unless proven
     * otherwise. */
     if (apity2ty[apitid] == tid + 1)
	return 1;
     apity2ty[apitid] = tid+1;

     if (ty->kind != apity->kind) {
	 cli_dbgmsg("bytecode: type kind mismatch: %u != %u\n", ty->kind, apity->kind);
	 return 0;
     }
     if (ty->numElements != apity->numElements) {
	 cli_dbgmsg("bytecode: type numElements mismatch: %u != %u\n", ty->numElements, apity->numElements);
	 return 0;
     }
     for (i=0;i<ty->numElements;i++) {
	if (apity->containedTypes[i] < BC_START_TID) {
	    if (ty->containedTypes[i] != apity->containedTypes[i]) {
		cli_dbgmsg("bytecode: contained type mismatch: %u != %u\n",
			   ty->containedTypes[i], apity->containedTypes[i]);
		return 0;
	    }
	} else if (!types_equal(bc, apity2ty, ty->containedTypes[i], apity->containedTypes[i] - BC_START_TID))
	    return 0;
	if (ty->kind == DArrayType)
	    break;/* validated the contained type already */
     }
    return 1;
}