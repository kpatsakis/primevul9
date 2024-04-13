OpTypeConstructor UnaryTensorContainer(FullTypeId t, FullTypeId dtype) {
  return [t, dtype](OpDef* op_def) {
    FullTypeDef* tdef =
        op_def->mutable_output_arg(0)->mutable_experimental_full_type();
    tdef->set_type_id(t);

    FullTypeDef* arg = tdef->add_args();
    arg->set_type_id(TFT_TENSOR);
    FullTypeDef* targ = arg->add_args();
    targ->set_type_id(dtype);

    return Status::OK();
  };
}