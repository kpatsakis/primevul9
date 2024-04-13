void cli_bytecode_context_set_trace(struct cli_bc_ctx* ctx, unsigned level,
				    bc_dbg_callback_trace trace,
				    bc_dbg_callback_trace_op trace_op,
				    bc_dbg_callback_trace_val trace_val,
				    bc_dbg_callback_trace_ptr trace_ptr)
{
    ctx->trace = trace;
    ctx->trace_op = trace_op;
    ctx->trace_val = trace_val;
    ctx->trace_ptr = trace_ptr;
    ctx->trace_level = level;
}