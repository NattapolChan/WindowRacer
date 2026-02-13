#pragma once

#include <stddef.h>
#include "event.hpp"
#include "XoshiroCpp.hpp"
#include "window_racer_config.hpp"
#include <thread>
#include <chrono>

using namespace std;

static exponential_distribution<double> delay_dist(phold_lambda);
static uniform_int_distribution<int> dst_dist(0, num_entities - 2); // truly remote
static uniform_real_distribution<double> uniform_dist;

static const int num_entities_per_thread = num_entities / num_threads;

class PHoldEntity {
public:
  int id;

  __int128 dummy_state[phold_events_per_entity - 1]; // one dummy rng state per entity (minus one to account for the actual RNG state)

  XoshiroCpp::Xoroshiro128StarStar gen;

  void schedule_event(int tid, double now) {
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    double new_ts;

    int dst_id = id;
    dst_id = (id + (int)(now / 0.5f)) % num_entities;
    new_ts = now + 0.0015 + (int)(now / 0.2f) * 0.02 + (dst_id % 5) * 0.025 + (id % 2) * phold_lookahead;
    if ( (int) ((new_ts - now) * 1000) % 2)
        dst_id = id;
    // if (uniform_dist(gen) < phold_zero_delay_ratio) {
    //    if (num_threads == 1)
    //      new_ts = now;
    //    else
    //      // new_ts = now;
    //      new_ts = nextafter(now, DBL_MAX);
    // } else {
    //    new_ts = now + phold_lookahead + delay_dist(gen);
    // }

    // if (uniform_dist(gen) < phold_remote_ratio)
    //   dst_id = (id + dst_dist(gen) + 1) % num_entities;
    
    Event::schedule(tid, id, now, new_ts, dst_id);
  }

  void init(int tid, int id) {
    this->id = id;
    gen.seed(random_seed ^ id);

    for (int i = 0; i < phold_events_per_entity; i++) {
      // for DEBUG
      // double new_ts = phold_lookahead + delay_dist(gen);
      double new_ts = (double)((id * 498345) % 12394) / 9234. + DBL_MIN;
      printf("scheduling %d %d %f %f %d\n", tid, id , 0.0, new_ts, id);
      Event::schedule(tid, id, 0.0, new_ts, id);
    }

  }

  void handle(int tid, double ts) {
    schedule_event(tid, ts);
  }
};
