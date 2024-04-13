void LinkInfo::print() {
  ResourceMark rm;
  tty->print_cr("Link resolved_klass=%s name=%s signature=%s current_klass=%s check_access=%s check_loader_constraints=%s",
                _resolved_klass->name()->as_C_string(),
                _name->as_C_string(),
                _signature->as_C_string(),
                _current_klass == NULL ? "(none)" : _current_klass->name()->as_C_string(),
                _check_access ? "true" : "false",
                _check_loader_constraints ? "true" : "false");

}