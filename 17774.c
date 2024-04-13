void InstanceKlass::release_C_heap_structures(InstanceKlass* ik) {
  // Clean up C heap
  ik->release_C_heap_structures();
  ik->constants()->release_C_heap_structures();
}