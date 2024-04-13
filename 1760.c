inline void CopyDimsToDesc(const RuntimeShape& input_shape,
                           NdArrayDesc<N>* desc_out) {
  int desc_stride = 1;
  for (int i = N - 1; i >= 0; --i) {
    desc_out->extents[i] = input_shape.Dims(i);
    desc_out->strides[i] = desc_stride;
    desc_stride *= input_shape.Dims(i);
  }
}