  explicit DenseCount(OpKernelConstruction* context) : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("minlength", &minlength_));
    OP_REQUIRES_OK(context, context->GetAttr("maxlength", &maxlength_));
    OP_REQUIRES_OK(context, context->GetAttr("binary_output", &binary_output_));
  }