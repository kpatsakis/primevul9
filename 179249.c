int vrend_create_sampler_view(struct vrend_context *ctx,
                              uint32_t handle,
                              uint32_t res_handle, uint32_t format,
                              uint32_t val0, uint32_t val1, uint32_t swizzle_packed)
{
   struct vrend_sampler_view *view;
   struct vrend_resource *res;
   int ret_handle;
   uint8_t swizzle[4];

   res = vrend_renderer_ctx_res_lookup(ctx, res_handle);
   if (!res) {
      report_context_error(ctx, VIRGL_ERROR_CTX_ILLEGAL_RESOURCE, res_handle);
      return EINVAL;
   }

   view = CALLOC_STRUCT(vrend_sampler_view);
   if (!view)
      return ENOMEM;

   pipe_reference_init(&view->reference, 1);
   view->format = format & 0xffffff;

   if (!view->format || (enum virgl_formats)view->format >= VIRGL_FORMAT_MAX) {
      report_context_error(ctx, VIRGL_ERROR_CTX_ILLEGAL_FORMAT, view->format);
      FREE(view);
      return EINVAL;
   }

   uint32_t pipe_target = (format >> 24) & 0xff;
   if (pipe_target >= PIPE_MAX_TEXTURE_TYPES) {
      report_context_error(ctx, VIRGL_ERROR_CTX_ILLEGAL_SAMPLER_VIEW_TARGET,
                           view->format);
      FREE(view);
      return EINVAL;
   }

   view->target = tgsitargettogltarget(pipe_target, res->base.nr_samples);

   /* Work around TEXTURE_RECTANGLE and TEXTURE_1D missing on GLES */
   if (vrend_state.use_gles) {
      if (view->target == GL_TEXTURE_RECTANGLE_NV ||
          view->target == GL_TEXTURE_1D)
         view->target = GL_TEXTURE_2D;
      else if (view->target == GL_TEXTURE_1D_ARRAY)
         view->target = GL_TEXTURE_2D_ARRAY;
   }

   view->val0 = val0;
   view->val1 = val1;

   swizzle[0] = swizzle_packed & 0x7;
   swizzle[1] = (swizzle_packed >> 3) & 0x7;
   swizzle[2] = (swizzle_packed >> 6) & 0x7;
   swizzle[3] = (swizzle_packed >> 9) & 0x7;

   vrend_resource_reference(&view->texture, res);

   view->id = view->texture->id;
   if (view->target == PIPE_BUFFER)
      view->target = view->texture->target;

   view->srgb_decode = GL_DECODE_EXT;
   if (view->format != view->texture->base.format) {
      if (util_format_is_srgb(view->texture->base.format) &&
          !util_format_is_srgb(view->format))
         view->srgb_decode = GL_SKIP_DECODE_EXT;
   }

   if (!(util_format_has_alpha(view->format) || util_format_is_depth_or_stencil(view->format))) {
      if (swizzle[0] == PIPE_SWIZZLE_ALPHA)
          swizzle[0] = PIPE_SWIZZLE_ONE;
      if (swizzle[1] == PIPE_SWIZZLE_ALPHA)
          swizzle[1] = PIPE_SWIZZLE_ONE;
      if (swizzle[2] == PIPE_SWIZZLE_ALPHA)
          swizzle[2] = PIPE_SWIZZLE_ONE;
      if (swizzle[3] == PIPE_SWIZZLE_ALPHA)
          swizzle[3] = PIPE_SWIZZLE_ONE;
   }

   if (tex_conv_table[view->format].flags & VIRGL_TEXTURE_NEED_SWIZZLE) {
      if (swizzle[0] <= PIPE_SWIZZLE_ALPHA)
         swizzle[0] = tex_conv_table[view->format].swizzle[swizzle[0]];
      if (swizzle[1] <= PIPE_SWIZZLE_ALPHA)
         swizzle[1] = tex_conv_table[view->format].swizzle[swizzle[1]];
      if (swizzle[2] <= PIPE_SWIZZLE_ALPHA)
         swizzle[2] = tex_conv_table[view->format].swizzle[swizzle[2]];
      if (swizzle[3] <= PIPE_SWIZZLE_ALPHA)
         swizzle[3] = tex_conv_table[view->format].swizzle[swizzle[3]];
   }

   view->gl_swizzle_r = to_gl_swizzle(swizzle[0]);
   view->gl_swizzle_g = to_gl_swizzle(swizzle[1]);
   view->gl_swizzle_b = to_gl_swizzle(swizzle[2]);
   view->gl_swizzle_a = to_gl_swizzle(swizzle[3]);

   if (has_feature(feat_texture_view) &&
       view->texture->storage != VREND_RESOURCE_STORAGE_BUFFER) {
      enum pipe_format format;
      bool needs_view = false;

      /*
       * Need to use a texture view if the gallium
       * view target is different than the underlying
       * texture target.
       */
      if (view->target != view->texture->target)
         needs_view = true;

      /*
       * If the formats are different and this isn't
       * a DS texture a view is required.
       * DS are special as they use different gallium
       * formats for DS views into a combined resource.
       * GL texture views can't be use for this, stencil
       * texturing is used instead. For DS formats
       * aways program the underlying DS format as a
       * view could be required for layers.
       */
      format = view->format;
      if (util_format_is_depth_or_stencil(view->texture->base.format))
         format = view->texture->base.format;
      else if (view->format != view->texture->base.format)
         needs_view = true;
      if (needs_view && (tex_conv_table[view->texture->base.format].flags & VIRGL_TEXTURE_CAN_TEXTURE_STORAGE)) {
        glGenTextures(1, &view->id);
        GLenum internalformat = tex_conv_table[format].internalformat;
        unsigned base_layer = view->val0 & 0xffff;
        unsigned max_layer = (view->val0 >> 16) & 0xffff;
        int base_level = view->val1 & 0xff;
        int max_level = (view->val1 >> 8) & 0xff;

        glTextureView(view->id, view->target, view->texture->id, internalformat,
                      base_level, (max_level - base_level) + 1,
                      base_layer, max_layer - base_layer + 1);

        glBindTexture(view->target, view->id);

        if (util_format_is_depth_or_stencil(view->format)) {
           if (vrend_state.use_core_profile == false) {
              /* setting depth texture mode is deprecated in core profile */
              if (view->depth_texture_mode != GL_RED) {
                 glTexParameteri(view->target, GL_DEPTH_TEXTURE_MODE, GL_RED);
                 view->depth_texture_mode = GL_RED;
              }
           }
           if (has_feature(feat_stencil_texturing)) {
              const struct util_format_description *desc = util_format_description(view->format);
              if (!util_format_has_depth(desc)) {
                 glTexParameteri(view->target, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);
              } else {
                 glTexParameteri(view->target, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
              }
           }
        }

        glTexParameteri(view->target, GL_TEXTURE_BASE_LEVEL, base_level);
        glTexParameteri(view->target, GL_TEXTURE_MAX_LEVEL, max_level);
        glTexParameteri(view->target, GL_TEXTURE_SWIZZLE_R, view->gl_swizzle_r);
        glTexParameteri(view->target, GL_TEXTURE_SWIZZLE_G, view->gl_swizzle_g);
        glTexParameteri(view->target, GL_TEXTURE_SWIZZLE_B, view->gl_swizzle_b);
        glTexParameteri(view->target, GL_TEXTURE_SWIZZLE_A, view->gl_swizzle_a);
        if (util_format_is_srgb(view->format) &&
            has_feature(feat_texture_srgb_decode)) {
           glTexParameteri(view->target, GL_TEXTURE_SRGB_DECODE_EXT,
                            view->srgb_decode);
        }
        glBindTexture(view->target, 0);
     }
   }

   ret_handle = vrend_renderer_object_insert(ctx, view, sizeof(*view), handle, VIRGL_OBJECT_SAMPLER_VIEW);
   if (ret_handle == 0) {
      FREE(view);
      return ENOMEM;
   }
   return 0;
}