JNIid* JNIid::find(int offset) {
  JNIid* current = this;
  while (current != NULL) {
    if (current->offset() == offset) return current;
    current = current->next();
  }
  return NULL;
}