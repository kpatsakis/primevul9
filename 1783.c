inline void NdArrayDescsForElementwiseBroadcast(
    const RuntimeShape& input0_shape, const RuntimeShape& input1_shape,
    const RuntimeShape& input2_shape, NdArrayDesc<N>* desc0_out,
    NdArrayDesc<N>* desc1_out, NdArrayDesc<N>* desc2_out) {
  TFLITE_DCHECK(desc0_out != nullptr);
  TFLITE_DCHECK(desc1_out != nullptr);
  TFLITE_DCHECK(desc2_out != nullptr);

  auto extended_input0_shape = RuntimeShape::ExtendedShape(N, input0_shape);
  auto extended_input1_shape = RuntimeShape::ExtendedShape(N, input1_shape);
  auto extended_input2_shape = RuntimeShape::ExtendedShape(N, input2_shape);

  // Copy dims to desc, calculating strides.
  CopyDimsToDesc<N>(extended_input0_shape, desc0_out);
  CopyDimsToDesc<N>(extended_input1_shape, desc1_out);
  CopyDimsToDesc<N>(extended_input2_shape, desc2_out);

  // Walk over each dimension. If the extents are equal do nothing.
  // Otherwise, set the desc with extent 1 to have extent equal to the other and
  // stride 0.
  for (int i = 0; i < N; ++i) {
    const int extent0 = extended_input0_shape.Dims(i);
    const int extent1 = extended_input1_shape.Dims(i);
    const int extent2 = extended_input2_shape.Dims(i);

    int extent = extent0;
    if (extent1 != 1) extent = extent1;
    if (extent2 != 1) extent = extent2;

    TFLITE_DCHECK(extent0 == 1 || extent0 == extent);
    TFLITE_DCHECK(extent1 == 1 || extent1 == extent);
    TFLITE_DCHECK(extent2 == 1 || extent2 == extent);

    if (!(extent0 == extent1 && extent1 == extent2)) {
      if (extent0 == 1) {
        desc0_out->strides[i] = 0;
        desc0_out->extents[i] = extent;
      }
      if (extent1 == 1) {
        desc1_out->strides[i] = 0;
        desc1_out->extents[i] = extent;
      }
      if (extent2 == 1) {
        desc2_out->strides[i] = 0;
        desc2_out->extents[i] = extent;
      }
    }
  }
}