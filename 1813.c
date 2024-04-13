OpTypeConstructor Nullary(FullTypeId t) {
  return [t](OpDef* op_def) {
    FullTypeDef* tdef =
        op_def->mutable_output_arg(0)->mutable_experimental_full_type();
    tdef->set_type_id(t);
    return Status::OK();
  };
}