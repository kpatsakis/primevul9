get_sw_value(buf_T *buf)
{
    return buf->b_p_sw ? buf->b_p_sw : buf->b_p_ts;
}