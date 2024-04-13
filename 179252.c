static int check_resource_valid(struct vrend_renderer_resource_create_args *args,
                                char errmsg[256])
{
   /* do not accept handle 0 */
   if (args->handle == 0) {
      snprintf(errmsg, 256, "Invalid handle");
      return -1;
   }

   /* limit the target */
   if (args->target >= PIPE_MAX_TEXTURE_TYPES) {
      snprintf(errmsg, 256, "Invalid texture target %d (>= %d)",
               args->target, PIPE_MAX_TEXTURE_TYPES);
      return -1;
   }

   if (args->format >= VIRGL_FORMAT_MAX) {
      snprintf(errmsg, 256, "Invalid texture format %d (>=%d)",
               args->format, VIRGL_FORMAT_MAX);
      return -1;
   }

   bool format_can_texture_storage = has_feature(feat_texture_storage) &&
         (tex_conv_table[args->format].flags & VIRGL_TEXTURE_CAN_TEXTURE_STORAGE);

   /* only texture 2d and 2d array can have multiple samples */
   if (args->nr_samples > 0) {
      if (!has_feature(feat_texture_multisample)) {
         snprintf(errmsg, 256, "Multisample textures not supported");
         return -1;
      }

      if (args->target != PIPE_TEXTURE_2D && args->target != PIPE_TEXTURE_2D_ARRAY) {
         snprintf(errmsg, 256, "Multisample textures not 2D (target:%d)", args->target);
         return -1;
      }
      /* multisample can't have miplevels */
      if (args->last_level > 0) {
         snprintf(errmsg, 256, "Multisample textures don't support mipmaps");
         return -1;
      }
   }

   if (args->last_level > 0) {
      /* buffer and rect textures can't have mipmaps */
      if (args->target == PIPE_BUFFER) {
         snprintf(errmsg, 256, "Buffers don't support mipmaps");
         return -1;
      }

      if (args->target == PIPE_TEXTURE_RECT) {
         snprintf(errmsg, 256, "RECT textures don't support mipmaps");
         return -1;
      }

      if (args->last_level > (floor(log2(MAX2(args->width, args->height))) + 1)) {
         snprintf(errmsg, 256, "Mipmap levels %d too large for texture size (%d, %d)",
                  args->last_level, args->width, args->height);
         return -1;
      }
   }

   if (args->flags != 0 && args->flags != VIRGL_RESOURCE_Y_0_TOP) {
      snprintf(errmsg, 256, "Resource flags 0x%x not supported", args->flags);
      return -1;
   }

   if (args->flags & VIRGL_RESOURCE_Y_0_TOP) {
      if (args->target != PIPE_TEXTURE_2D && args->target != PIPE_TEXTURE_RECT) {
         snprintf(errmsg, 256, "VIRGL_RESOURCE_Y_0_TOP only supported for 2D or RECT textures");
         return -1;
      }
   }

   /* array size for array textures only */
   if (args->target == PIPE_TEXTURE_CUBE) {
      if (args->array_size != 6) {
         snprintf(errmsg, 256, "Cube map: unexpected array size %d", args->array_size);
         return -1;
      }
   } else if (args->target == PIPE_TEXTURE_CUBE_ARRAY) {
      if (!has_feature(feat_cube_map_array)) {
         snprintf(errmsg, 256, "Cube map arrays not supported");
         return -1;
      }
      if (args->array_size % 6) {
         snprintf(errmsg, 256, "Cube map array: unexpected array size %d", args->array_size);
         return -1;
      }
   } else if (args->array_size > 1) {
      if (args->target != PIPE_TEXTURE_2D_ARRAY &&
          args->target != PIPE_TEXTURE_1D_ARRAY) {
         snprintf(errmsg, 256, "Texture target %d can't be an array ", args->target);
         return -1;
      }

      if (!has_feature(feat_texture_array)) {
         snprintf(errmsg, 256, "Texture arrays are not supported");
         return -1;
      }
   }

   if (format_can_texture_storage && !args->width) {
      snprintf(errmsg, 256, "Texture storage texture width must be >0");
      return -1;
   }

   if (args->bind == 0 ||
       args->bind == VIRGL_BIND_CUSTOM ||
       args->bind == VIRGL_BIND_STAGING ||
       args->bind == VIRGL_BIND_INDEX_BUFFER ||
       args->bind == VIRGL_BIND_STREAM_OUTPUT ||
       args->bind == VIRGL_BIND_VERTEX_BUFFER ||
       args->bind == VIRGL_BIND_CONSTANT_BUFFER ||
       args->bind == VIRGL_BIND_QUERY_BUFFER ||
       args->bind == VIRGL_BIND_COMMAND_ARGS ||
       args->bind == VIRGL_BIND_SHADER_BUFFER) {
      if (args->target != PIPE_BUFFER) {
         snprintf(errmsg, 256, "Buffer bind flags requre the buffer target but this is target %d", args->target);
         return -1;
      }
      if (args->height != 1 || args->depth != 1) {
         snprintf(errmsg, 256, "Buffer target: Got height=%u, depth=%u, expect (1,1)", args->height, args->depth);
         return -1;
      }
      if (args->bind == VIRGL_BIND_QUERY_BUFFER && !has_feature(feat_qbo)) {
         snprintf(errmsg, 256, "Query buffers are not supported");
         return -1;
      }
      if (args->bind == VIRGL_BIND_COMMAND_ARGS && !has_feature(feat_indirect_draw)) {
         snprintf(errmsg, 256, "Command args buffer requested but indirect draw is not supported");
         return -1;
      }
   } else {
      if (!((args->bind & VIRGL_BIND_SAMPLER_VIEW) ||
            (args->bind & VIRGL_BIND_DEPTH_STENCIL) ||
            (args->bind & VIRGL_BIND_RENDER_TARGET) ||
            (args->bind & VIRGL_BIND_CURSOR) ||
            (args->bind & VIRGL_BIND_SHARED) ||
            (args->bind & VIRGL_BIND_LINEAR))) {
         snprintf(errmsg, 256, "Invalid texture bind flags 0x%x", args->bind);
         return -1;
      }

      if (args->target == PIPE_TEXTURE_2D ||
          args->target == PIPE_TEXTURE_RECT ||
          args->target == PIPE_TEXTURE_CUBE ||
          args->target == PIPE_TEXTURE_2D_ARRAY ||
          args->target == PIPE_TEXTURE_CUBE_ARRAY) {
         if (args->depth != 1) {
            snprintf(errmsg, 256, "2D texture target with depth=%u != 1", args->depth);
            return -1;
         }
         if (format_can_texture_storage && !args->height) {
            snprintf(errmsg, 256, "2D Texture storage requires non-zero height");
            return -1;
         }
      }
      if (args->target == PIPE_TEXTURE_1D ||
          args->target == PIPE_TEXTURE_1D_ARRAY) {
         if (args->height != 1 || args->depth != 1) {
            snprintf(errmsg, 256, "Got height=%u, depth=%u, expect (1,1)",
                     args->height, args->depth);
            return -1;
         }
         if (args->width > vrend_state.max_texture_2d_size) {
            snprintf(errmsg, 256, "1D Texture width (%u) exceeds supported value (%u)",
                     args->width, vrend_state.max_texture_2d_size);
            return -1;
         }
      }

      if (args->target == PIPE_TEXTURE_2D ||
          args->target == PIPE_TEXTURE_RECT ||
          args->target == PIPE_TEXTURE_2D_ARRAY) {
         if (args->width > vrend_state.max_texture_2d_size ||
             args->height > vrend_state.max_texture_2d_size) {
            snprintf(errmsg, 256, "2D Texture size components (%u, %u) exceeds supported value (%u)",
                     args->width, args->height, vrend_state.max_texture_2d_size);
            return -1;
         }
      }

      if (args->target == PIPE_TEXTURE_3D) {
         if (format_can_texture_storage &&
             (!args->height || !args->depth)) {
            snprintf(errmsg, 256, "Texture storage expects non-zero height (%u) and depth (%u)",
                     args->height, args->depth);
            return -1;
         }
         if (args->width > vrend_state.max_texture_3d_size ||
             args->height > vrend_state.max_texture_3d_size ||
             args->depth > vrend_state.max_texture_3d_size) {
            snprintf(errmsg, 256, "3D Texture sizes (%u, %u, %u) exceeds supported value (%u)",
                     args->width, args->height, args->depth,
                     vrend_state.max_texture_3d_size);
            return -1;
         }
      }
      if (args->target == PIPE_TEXTURE_2D_ARRAY ||
          args->target == PIPE_TEXTURE_CUBE_ARRAY ||
          args->target == PIPE_TEXTURE_1D_ARRAY) {
         if (format_can_texture_storage &&
             !args->array_size) {
            snprintf(errmsg, 256, "Texture arrays require a non-zero arrays size "
                                  "when allocated with glTexStorage");
            return -1;
         }
      }
      if (args->target == PIPE_TEXTURE_CUBE ||
          args->target == PIPE_TEXTURE_CUBE_ARRAY) {
         if (args->width != args->height) {
            snprintf(errmsg, 256, "Cube maps require width (%u) == height (%u)",
                     args->width, args->height);
            return -1;
         }
         if (args->width > vrend_state.max_texture_cube_size) {
            snprintf(errmsg, 256, "Cube maps size (%u) exceeds supported value (%u)",
                     args->width, vrend_state.max_texture_cube_size);
            return -1;
         }
      }
   }
   return 0;
}