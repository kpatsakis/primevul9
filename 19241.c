LinkInfo::LinkInfo(const constantPoolHandle& pool, int index, const methodHandle& current_method, TRAPS) {
   // resolve klass
  _resolved_klass = pool->klass_ref_at(index, CHECK);

  // Get name, signature, and static klass
  _name          = pool->name_ref_at(index);
  _signature     = pool->signature_ref_at(index);
  _tag           = pool->tag_ref_at(index);
  _current_klass = pool->pool_holder();
  _current_method = current_method;

  // Coming from the constant pool always checks access
  _check_access  = true;
  _check_loader_constraints = true;
}