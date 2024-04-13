void FieldPrinter::do_field(fieldDescriptor* fd) {
  _st->print(BULLET);
   if (_obj == NULL) {
     fd->print_on(_st);
     _st->cr();
   } else {
     fd->print_on_for(_st, _obj);
     _st->cr();
   }
}