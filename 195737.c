static inline int get_geptypesize(const struct cli_bc *bc, uint16_t tid)
{
  const struct cli_bc_type *ty;
  if (tid >= bc->num_types+65) {
    cli_errmsg("bytecode: typeid out of range %u >= %u\n", tid, bc->num_types);
    return -1;
  }
  if (tid <= 64) {
    cli_errmsg("bytecode: invalid type for gep (%u)\n", tid);
    return -1;
  }
  ty = &bc->types[tid - 65];
  if (ty->kind != DPointerType) {
    cli_errmsg("bytecode: invalid gep type, must be pointer: %u\n", tid);
    return -1;
  }
  return typesize(bc, ty->containedTypes[0]);
}