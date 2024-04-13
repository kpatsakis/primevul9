const char* InstanceKlass::nest_host_error() {
  if (_nest_host_index == 0) {
    return NULL;
  } else {
    constantPoolHandle cph(Thread::current(), constants());
    return SystemDictionary::find_nest_host_error(cph, (int)_nest_host_index);
  }
}