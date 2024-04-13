    CImg<typename cimg::superset<t,long>::type>
    get_distance_dijkstra(const T& value, const CImg<t>& metric, const bool is_high_connectivity,
                          CImg<to>& return_path) const {
      if (is_empty()) return return_path.assign();
      if (!is_sameXYZ(metric))
        throw CImgArgumentException(_cimg_instance
                                    "distance_dijkstra(): image instance and metric map (%u,%u,%u,%u) "
                                    "have incompatible dimensions.",
                                    cimg_instance,
                                    metric._width,metric._height,metric._depth,metric._spectrum);
      typedef typename cimg::superset<t,long>::type td;  // Type used for computing cumulative distances.
      CImg<td> result(_width,_height,_depth,_spectrum), Q;
      CImg<boolT> is_queued(_width,_height,_depth,1);
      if (return_path) return_path.assign(_width,_height,_depth,_spectrum);

      cimg_forC(*this,c) {
        const CImg<T> img = get_shared_channel(c);
        const CImg<t> met = metric.get_shared_channel(c%metric._spectrum);
        CImg<td> res = result.get_shared_channel(c);
        CImg<to> path = return_path?return_path.get_shared_channel(c):CImg<to>();
        unsigned int sizeQ = 0;

        // Detect initial seeds.
        is_queued.fill(0);
        cimg_forXYZ(img,x,y,z) if (img(x,y,z)==value) {
          Q._priority_queue_insert(is_queued,sizeQ,0,x,y,z);
          res(x,y,z) = 0;
          if (path) path(x,y,z) = (to)0;
        }

        // Start distance propagation.
        while (sizeQ) {

          // Get and remove point with minimal potential from the queue.
          const int x = (int)Q(0,1), y = (int)Q(0,2), z = (int)Q(0,3);
          const td P = (td)-Q(0,0);
          Q._priority_queue_remove(sizeQ);

          // Update neighbors.
          td npot = 0;
          if (x - 1>=0 && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x - 1,y,z) + P),x - 1,y,z)) {
            res(x - 1,y,z) = npot; if (path) path(x - 1,y,z) = (to)2;
          }
          if (x + 1<width() && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x + 1,y,z) + P),x + 1,y,z)) {
            res(x + 1,y,z) = npot; if (path) path(x + 1,y,z) = (to)1;
          }
          if (y - 1>=0 && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x,y - 1,z) + P),x,y - 1,z)) {
            res(x,y - 1,z) = npot; if (path) path(x,y - 1,z) = (to)8;
          }
          if (y + 1<height() && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x,y + 1,z) + P),x,y + 1,z)) {
            res(x,y + 1,z) = npot; if (path) path(x,y + 1,z) = (to)4;
          }
          if (z - 1>=0 && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x,y,z - 1) + P),x,y,z - 1)) {
            res(x,y,z - 1) = npot; if (path) path(x,y,z - 1) = (to)32;
          }
          if (z + 1<depth() && Q._priority_queue_insert(is_queued,sizeQ,-(npot=met(x,y,z + 1) + P),x,y,z + 1)) {
            res(x,y,z + 1) = npot; if (path) path(x,y,z + 1) = (to)16;
          }

          if (is_high_connectivity) {
            const float sqrt2 = std::sqrt(2.0f), sqrt3 = std::sqrt(3.0f);

            // Diagonal neighbors on slice z.
            if (x - 1>=0 && y - 1>=0 &&
                Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x - 1,y - 1,z) + P)),x - 1,y - 1,z)) {
              res(x - 1,y - 1,z) = npot; if (path) path(x - 1,y - 1,z) = (to)10;
            }
            if (x + 1<width() && y - 1>=0 &&
                Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x + 1,y - 1,z) + P)),x + 1,y - 1,z)) {
              res(x + 1,y - 1,z) = npot; if (path) path(x + 1,y - 1,z) = (to)9;
            }
            if (x - 1>=0 && y + 1<height() &&
                Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x - 1,y + 1,z) + P)),x - 1,y + 1,z)) {
              res(x - 1,y + 1,z) = npot; if (path) path(x - 1,y + 1,z) = (to)6;
            }
            if (x + 1<width() && y + 1<height() &&
                Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x + 1,y + 1,z) + P)),x + 1,y + 1,z)) {
              res(x + 1,y + 1,z) = npot; if (path) path(x + 1,y + 1,z) = (to)5;
            }

            if (z - 1>=0) { // Diagonal neighbors on slice z - 1.
              if (x - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x - 1,y,z - 1) + P)),x - 1,y,z - 1)) {
                res(x - 1,y,z - 1) = npot; if (path) path(x - 1,y,z - 1) = (to)34;
              }
              if (x + 1<width() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x + 1,y,z - 1) + P)),x + 1,y,z - 1)) {
                res(x + 1,y,z - 1) = npot; if (path) path(x + 1,y,z - 1) = (to)33;
              }
              if (y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x,y - 1,z - 1) + P)),x,y - 1,z - 1)) {
                res(x,y - 1,z - 1) = npot; if (path) path(x,y - 1,z - 1) = (to)40;
              }
              if (y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x,y + 1,z - 1) + P)),x,y + 1,z - 1)) {
                res(x,y + 1,z - 1) = npot; if (path) path(x,y + 1,z - 1) = (to)36;
              }
              if (x - 1>=0 && y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x - 1,y - 1,z - 1) + P)),
                                           x - 1,y - 1,z - 1)) {
                res(x - 1,y - 1,z - 1) = npot; if (path) path(x - 1,y - 1,z - 1) = (to)42;
              }
              if (x + 1<width() && y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x + 1,y - 1,z - 1) + P)),
                                           x + 1,y - 1,z - 1)) {
                res(x + 1,y - 1,z - 1) = npot; if (path) path(x + 1,y - 1,z - 1) = (to)41;
              }
              if (x - 1>=0 && y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x - 1,y + 1,z - 1) + P)),
                                           x - 1,y + 1,z - 1)) {
                res(x - 1,y + 1,z - 1) = npot; if (path) path(x - 1,y + 1,z - 1) = (to)38;
              }
              if (x + 1<width() && y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x + 1,y + 1,z - 1) + P)),
                                           x + 1,y + 1,z - 1)) {
                res(x + 1,y + 1,z - 1) = npot; if (path) path(x + 1,y + 1,z - 1) = (to)37;
              }
            }

            if (z + 1<depth()) { // Diagonal neighbors on slice z + 1.
              if (x - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x - 1,y,z + 1) + P)),x - 1,y,z + 1)) {
                res(x - 1,y,z + 1) = npot; if (path) path(x - 1,y,z + 1) = (to)18;
              }
              if (x + 1<width() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x + 1,y,z + 1) + P)),x + 1,y,z + 1)) {
                res(x + 1,y,z + 1) = npot; if (path) path(x + 1,y,z + 1) = (to)17;
              }
              if (y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x,y - 1,z + 1) + P)),x,y - 1,z + 1)) {
                res(x,y - 1,z + 1) = npot; if (path) path(x,y - 1,z + 1) = (to)24;
              }
              if (y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt2*met(x,y + 1,z + 1) + P)),x,y + 1,z + 1)) {
                res(x,y + 1,z + 1) = npot; if (path) path(x,y + 1,z + 1) = (to)20;
              }
              if (x - 1>=0 && y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x - 1,y - 1,z + 1) + P)),
                                           x - 1,y - 1,z + 1)) {
                res(x - 1,y - 1,z + 1) = npot; if (path) path(x - 1,y - 1,z + 1) = (to)26;
              }
              if (x + 1<width() && y - 1>=0 &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x + 1,y - 1,z + 1) + P)),
                                           x + 1,y - 1,z + 1)) {
                res(x + 1,y - 1,z + 1) = npot; if (path) path(x + 1,y - 1,z + 1) = (to)25;
              }
              if (x - 1>=0 && y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x - 1,y + 1,z + 1) + P)),
                                           x - 1,y + 1,z + 1)) {
                res(x - 1,y + 1,z + 1) = npot; if (path) path(x - 1,y + 1,z + 1) = (to)22;
              }
              if (x + 1<width() && y + 1<height() &&
                  Q._priority_queue_insert(is_queued,sizeQ,-(npot=(td)(sqrt3*met(x + 1,y + 1,z + 1) + P)),
                                           x + 1,y + 1,z + 1)) {
                res(x + 1,y + 1,z + 1) = npot; if (path) path(x + 1,y + 1,z + 1) = (to)21;
              }
            }
          }
        }
      }
      return result;