  static void sass_clear_context (struct Sass_Context* ctx)
  {
    if (ctx == 0) return;
    // release the allocated memory (mostly via sass_copy_c_string)
    if (ctx->output_string)     free(ctx->output_string);
    if (ctx->source_map_string) free(ctx->source_map_string);
    if (ctx->error_message)     free(ctx->error_message);
    if (ctx->error_text)        free(ctx->error_text);
    if (ctx->error_json)        free(ctx->error_json);
    if (ctx->error_file)        free(ctx->error_file);
    free_string_array(ctx->included_files);
    // play safe and reset properties
    ctx->output_string = 0;
    ctx->source_map_string = 0;
    ctx->error_message = 0;
    ctx->error_text = 0;
    ctx->error_json = 0;
    ctx->error_file = 0;
    ctx->included_files = 0;
    // debug leaked memory
    #ifdef DEBUG_SHARED_PTR
      SharedObj::dumpMemLeaks();
    #endif
    // now clear the options
    sass_clear_options(ctx);
  }