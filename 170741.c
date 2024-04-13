void ClassHierarchyIterator::next() {
  assert(_current != NULL, "required");
  if (_visit_subclasses && _current->subklass() != NULL) {
    _current = _current->subklass();
    return; // visit next subclass
  }
  _visit_subclasses = true; // reset
  while (_current->next_sibling() == NULL && _current != _root) {
    _current = _current->superklass(); // backtrack; no more sibling subclasses left
  }
  if (_current == _root) {
    // Iteration is over (back at root after backtracking). Invalidate the iterator.
    _current = NULL;
    return;
  }
  _current = _current->next_sibling();
  return; // visit next sibling subclass
}