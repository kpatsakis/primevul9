void InstanceKlass::release_C_heap_structures() {

  // Clean up C heap
  release_C_heap_structures_internal();
  constants()->release_C_heap_structures();

  // Deallocate and call destructors for MDO mutexes
  methods_do(method_release_C_heap_structures);
}