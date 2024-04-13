void InstanceKlass::purge_previous_version_list() {
  assert(SafepointSynchronize::is_at_safepoint(), "only called at safepoint");
  assert(has_been_redefined(), "Should only be called for main class");

  // Quick exit.
  if (previous_versions() == NULL) {
    return;
  }

  // This klass has previous versions so see what we can cleanup
  // while it is safe to do so.

  int deleted_count = 0;    // leave debugging breadcrumbs
  int live_count = 0;
  ClassLoaderData* loader_data = class_loader_data();
  assert(loader_data != NULL, "should never be null");

  ResourceMark rm;
  log_trace(redefine, class, iklass, purge)("%s: previous versions", external_name());

  // previous versions are linked together through the InstanceKlass
  InstanceKlass* pv_node = previous_versions();
  InstanceKlass* last = this;
  int version = 0;

  // check the previous versions list
  for (; pv_node != NULL; ) {

    ConstantPool* pvcp = pv_node->constants();
    assert(pvcp != NULL, "cp ref was unexpectedly cleared");

    if (!pvcp->on_stack()) {
      // If the constant pool isn't on stack, none of the methods
      // are executing.  Unlink this previous_version.
      // The previous version InstanceKlass is on the ClassLoaderData deallocate list
      // so will be deallocated during the next phase of class unloading.
      log_trace(redefine, class, iklass, purge)
        ("previous version " INTPTR_FORMAT " is dead.", p2i(pv_node));
      // For debugging purposes.
      pv_node->set_is_scratch_class();
      // Unlink from previous version list.
      assert(pv_node->class_loader_data() == loader_data, "wrong loader_data");
      InstanceKlass* next = pv_node->previous_versions();
      pv_node->link_previous_versions(NULL);   // point next to NULL
      last->link_previous_versions(next);
      // Add to the deallocate list after unlinking
      loader_data->add_to_deallocate_list(pv_node);
      pv_node = next;
      deleted_count++;
      version++;
      continue;
    } else {
      log_trace(redefine, class, iklass, purge)("previous version " INTPTR_FORMAT " is alive", p2i(pv_node));
      assert(pvcp->pool_holder() != NULL, "Constant pool with no holder");
      guarantee (!loader_data->is_unloading(), "unloaded classes can't be on the stack");
      live_count++;
      // found a previous version for next time we do class unloading
      _has_previous_versions = true;
    }

    // At least one method is live in this previous version.
    // Reset dead EMCP methods not to get breakpoints.
    // All methods are deallocated when all of the methods for this class are no
    // longer running.
    Array<Method*>* method_refs = pv_node->methods();
    if (method_refs != NULL) {
      log_trace(redefine, class, iklass, purge)("previous methods length=%d", method_refs->length());
      for (int j = 0; j < method_refs->length(); j++) {
        Method* method = method_refs->at(j);

        if (!method->on_stack()) {
          // no breakpoints for non-running methods
          if (method->is_running_emcp()) {
            method->set_running_emcp(false);
          }
        } else {
          assert (method->is_obsolete() || method->is_running_emcp(),
                  "emcp method cannot run after emcp bit is cleared");
          log_trace(redefine, class, iklass, purge)
            ("purge: %s(%s): prev method @%d in version @%d is alive",
             method->name()->as_C_string(), method->signature()->as_C_string(), j, version);
        }
      }
    }
    // next previous version
    last = pv_node;
    pv_node = pv_node->previous_versions();
    version++;
  }
  log_trace(redefine, class, iklass, purge)
    ("previous version stats: live=%d, deleted=%d", live_count, deleted_count);
}