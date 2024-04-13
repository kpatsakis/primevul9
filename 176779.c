static CURLcode readwrite_upload(struct Curl_easy *data,
                                 struct connectdata *conn,
                                 int *didwhat)
{
  ssize_t i, si;
  ssize_t bytes_written;
  CURLcode result;
  ssize_t nread; /* number of bytes read */
  bool sending_http_headers = FALSE;
  struct SingleRequest *k = &data->req;

  if((k->bytecount == 0) && (k->writebytecount == 0))
    Curl_pgrsTime(data, TIMER_STARTTRANSFER);

  *didwhat |= KEEP_SEND;

  do {
    curl_off_t nbody;

    /* only read more data if there's no upload data already
       present in the upload buffer */
    if(0 == k->upload_present) {
      result = Curl_get_upload_buffer(data);
      if(result)
        return result;
      /* init the "upload from here" pointer */
      k->upload_fromhere = data->state.ulbuf;

      if(!k->upload_done) {
        /* HTTP pollution, this should be written nicer to become more
           protocol agnostic. */
        size_t fillcount;
        struct HTTP *http = k->p.http;

        if((k->exp100 == EXP100_SENDING_REQUEST) &&
           (http->sending == HTTPSEND_BODY)) {
          /* If this call is to send body data, we must take some action:
             We have sent off the full HTTP 1.1 request, and we shall now
             go into the Expect: 100 state and await such a header */
          k->exp100 = EXP100_AWAITING_CONTINUE; /* wait for the header */
          k->keepon &= ~KEEP_SEND;         /* disable writing */
          k->start100 = Curl_now();       /* timeout count starts now */
          *didwhat &= ~KEEP_SEND;  /* we didn't write anything actually */
          /* set a timeout for the multi interface */
          Curl_expire(data, data->set.expect_100_timeout, EXPIRE_100_TIMEOUT);
          break;
        }

        if(conn->handler->protocol&(PROTO_FAMILY_HTTP|CURLPROTO_RTSP)) {
          if(http->sending == HTTPSEND_REQUEST)
            /* We're sending the HTTP request headers, not the data.
               Remember that so we don't change the line endings. */
            sending_http_headers = TRUE;
          else
            sending_http_headers = FALSE;
        }

        result = Curl_fillreadbuffer(data, data->set.upload_buffer_size,
                                     &fillcount);
        if(result)
          return result;

        nread = fillcount;
      }
      else
        nread = 0; /* we're done uploading/reading */

      if(!nread && (k->keepon & KEEP_SEND_PAUSE)) {
        /* this is a paused transfer */
        break;
      }
      if(nread <= 0) {
        result = Curl_done_sending(data, k);
        if(result)
          return result;
        break;
      }

      /* store number of bytes available for upload */
      k->upload_present = nread;

      /* convert LF to CRLF if so asked */
      if((!sending_http_headers) && (
#ifdef CURL_DO_LINEEND_CONV
         /* always convert if we're FTPing in ASCII mode */
         (data->state.prefer_ascii) ||
#endif
         (data->set.crlf))) {
        /* Do we need to allocate a scratch buffer? */
        if(!data->state.scratch) {
          data->state.scratch = malloc(2 * data->set.upload_buffer_size);
          if(!data->state.scratch) {
            failf(data, "Failed to alloc scratch buffer!");

            return CURLE_OUT_OF_MEMORY;
          }
        }

        /*
         * ASCII/EBCDIC Note: This is presumably a text (not binary)
         * transfer so the data should already be in ASCII.
         * That means the hex values for ASCII CR (0x0d) & LF (0x0a)
         * must be used instead of the escape sequences \r & \n.
         */
        for(i = 0, si = 0; i < nread; i++, si++) {
          if(k->upload_fromhere[i] == 0x0a) {
            data->state.scratch[si++] = 0x0d;
            data->state.scratch[si] = 0x0a;
            if(!data->set.crlf) {
              /* we're here only because FTP is in ASCII mode...
                 bump infilesize for the LF we just added */
              if(data->state.infilesize != -1)
                data->state.infilesize++;
            }
          }
          else
            data->state.scratch[si] = k->upload_fromhere[i];
        }

        if(si != nread) {
          /* only perform the special operation if we really did replace
             anything */
          nread = si;

          /* upload from the new (replaced) buffer instead */
          k->upload_fromhere = data->state.scratch;

          /* set the new amount too */
          k->upload_present = nread;
        }
      }

#ifndef CURL_DISABLE_SMTP
      if(conn->handler->protocol & PROTO_FAMILY_SMTP) {
        result = Curl_smtp_escape_eob(data, nread);
        if(result)
          return result;
      }
#endif /* CURL_DISABLE_SMTP */
    } /* if 0 == k->upload_present */
    else {
      /* We have a partial buffer left from a previous "round". Use
         that instead of reading more data */
    }

    /* write to socket (send away data) */
    result = Curl_write(data,
                        conn->writesockfd,  /* socket to send to */
                        k->upload_fromhere, /* buffer pointer */
                        k->upload_present,  /* buffer size */
                        &bytes_written);    /* actually sent */
    if(result)
      return result;

    win_update_buffer_size(conn->writesockfd);

    if(k->pendingheader) {
      /* parts of what was sent was header */
      curl_off_t n = CURLMIN(k->pendingheader, bytes_written);
      /* show the data before we change the pointer upload_fromhere */
      Curl_debug(data, CURLINFO_HEADER_OUT, k->upload_fromhere, (size_t)n);
      k->pendingheader -= n;
      nbody = bytes_written - n; /* size of the written body part */
    }
    else
      nbody = bytes_written;

    if(nbody) {
      /* show the data before we change the pointer upload_fromhere */
      Curl_debug(data, CURLINFO_DATA_OUT,
                 &k->upload_fromhere[bytes_written - nbody],
                 (size_t)nbody);

      k->writebytecount += nbody;
      Curl_pgrsSetUploadCounter(data, k->writebytecount);
    }

    if((!k->upload_chunky || k->forbidchunk) &&
       (k->writebytecount == data->state.infilesize)) {
      /* we have sent all data we were supposed to */
      k->upload_done = TRUE;
      infof(data, "We are completely uploaded and fine\n");
    }

    if(k->upload_present != bytes_written) {
      /* we only wrote a part of the buffer (if anything), deal with it! */

      /* store the amount of bytes left in the buffer to write */
      k->upload_present -= bytes_written;

      /* advance the pointer where to find the buffer when the next send
         is to happen */
      k->upload_fromhere += bytes_written;
    }
    else {
      /* we've uploaded that buffer now */
      result = Curl_get_upload_buffer(data);
      if(result)
        return result;
      k->upload_fromhere = data->state.ulbuf;
      k->upload_present = 0; /* no more bytes left */

      if(k->upload_done) {
        result = Curl_done_sending(data, k);
        if(result)
          return result;
      }
    }


  } while(0); /* just to break out from! */

  return CURLE_OK;
}