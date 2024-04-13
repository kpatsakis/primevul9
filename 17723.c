  template <class T> void do_oop_work(T* p) {
    oop obj = RawAccess<>::oop_load(p);
    if (!oopDesc::is_oop_or_null(obj)) {
      tty->print_cr("Failed: " PTR_FORMAT " -> " PTR_FORMAT, p2i(p), p2i(obj));
      Universe::print_on(tty);
      guarantee(false, "boom");
    }
  }