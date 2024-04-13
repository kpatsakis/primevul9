qt_demux_state_string (enum QtDemuxState state)
{
  switch (state) {
    case QTDEMUX_STATE_INITIAL:
      return "<INITIAL>";
    case QTDEMUX_STATE_HEADER:
      return "<HEADER>";
    case QTDEMUX_STATE_MOVIE:
      return "<MOVIE>";
    case QTDEMUX_STATE_BUFFER_MDAT:
      return "<BUFFER_MDAT>";
    default:
      return "<UNKNOWN>";
  }
}