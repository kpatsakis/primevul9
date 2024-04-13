void Curl_getoff_all_pipelines(struct Curl_easy *data,
                               struct connectdata *conn)
{
  bool recv_head = (conn->readchannel_inuse &&
                    Curl_recvpipe_head(data, conn));
  bool send_head = (conn->writechannel_inuse &&
                    Curl_sendpipe_head(data, conn));

  if(Curl_removeHandleFromPipeline(data, &conn->recv_pipe) && recv_head)
    Curl_pipeline_leave_read(conn);
  if(Curl_removeHandleFromPipeline(data, &conn->send_pipe) && send_head)
    Curl_pipeline_leave_write(conn);
}