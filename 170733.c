void InstanceKlass::oop_print_value_on(oop obj, outputStream* st) {
  st->print("a ");
  name()->print_value_on(st);
  obj->print_address_on(st);
  if (this == vmClasses::String_klass()
      && java_lang_String::value(obj) != NULL) {
    ResourceMark rm;
    int len = java_lang_String::length(obj);
    int plen = (len < 24 ? len : 12);
    char* str = java_lang_String::as_utf8_string(obj, 0, plen);
    st->print(" = \"%s\"", str);
    if (len > plen)
      st->print("...[%d]", len);
  } else if (this == vmClasses::Class_klass()) {
    Klass* k = java_lang_Class::as_Klass(obj);
    st->print(" = ");
    if (k != NULL) {
      k->print_value_on(st);
    } else {
      const char* tname = type2name(java_lang_Class::primitive_type(obj));
      st->print("%s", tname ? tname : "type?");
    }
  } else if (this == vmClasses::MethodType_klass()) {
    st->print(" = ");
    java_lang_invoke_MethodType::print_signature(obj, st);
  } else if (java_lang_boxing_object::is_instance(obj)) {
    st->print(" = ");
    java_lang_boxing_object::print(obj, st);
  } else if (this == vmClasses::LambdaForm_klass()) {
    oop vmentry = java_lang_invoke_LambdaForm::vmentry(obj);
    if (vmentry != NULL) {
      st->print(" => ");
      vmentry->print_value_on(st);
    }
  } else if (this == vmClasses::MemberName_klass()) {
    Metadata* vmtarget = java_lang_invoke_MemberName::vmtarget(obj);
    if (vmtarget != NULL) {
      st->print(" = ");
      vmtarget->print_value_on(st);
    } else {
      oop clazz = java_lang_invoke_MemberName::clazz(obj);
      oop name  = java_lang_invoke_MemberName::name(obj);
      if (clazz != NULL) {
        clazz->print_value_on(st);
      } else {
        st->print("NULL");
      }
      st->print(".");
      if (name != NULL) {
        name->print_value_on(st);
      } else {
        st->print("NULL");
      }
    }
  }
}