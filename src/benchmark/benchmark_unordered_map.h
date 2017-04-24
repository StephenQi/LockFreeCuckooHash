#ifndef BENCHMARK_UNORDERED_MAP
#define BENCHMARK_UNORDERED_MAP

#include <unordered_map>
#include <iostream>
#include <random>
#include <algorithm>

#include "../common/cycle_timer.h"

#define NUM_ITERS 3

class BenchmarkUnorderedMap
{
  public:
    BenchmarkUnorderedMap(int op_count, int capacity, 
                          int rweight, int idweight, 
                          double load_factor);

    void benchmark_all();
    void run();
  private:
    int    m_rweight;
    int    m_idweight;

    int    m_op_count;
    int    m_capacity;
    double m_load_factor;
};

BenchmarkUnorderedMap::BenchmarkUnorderedMap(int op_count, int capacity, 
                                             int rweight, int idweight,
                                             double load_factor)
{
  std::cout << "*** BENCHMARKING UnorderedMap ***" << std::endl;
  m_op_count    = op_count;
  m_load_factor = load_factor; 
  m_capacity    = capacity;

  m_rweight     = rweight;
  m_idweight    = idweight;
}

void BenchmarkUnorderedMap::benchmark_all()
{
    std::unordered_map<int, int> map;
    map.reserve(m_capacity);

    std::random_device                 rd;
    std::mt19937                       mt(rd());
    std::uniform_int_distribution<int> rng;

    std::array<int, 3> weights;
    weights[0] = m_rweight;
    weights[1] = m_idweight;
    weights[2] = m_idweight;

    std::default_random_engine         g;
    std::discrete_distribution<int>    drng(weights.begin(), weights.end());

    // Warm-up table to load factor
    int num_warmup = static_cast<int>(static_cast<double>(m_capacity) * m_load_factor);
    for (int i = 0; i < num_warmup; i++)
    {
      int k = rng(mt); 
      int v = rng(mt);
      map[k] = v;
    }

    // Run benchmark (single-threaded)
    std::vector<double> results;
    for (int iter = 0; iter < NUM_ITERS; iter++)
    {
      double start = CycleTimer::currentSeconds();
      for (int i = 0; i < m_op_count; i++)
      {
        int k = rng(mt);
        int v = rng(mt);
        int a = drng(g);

        if (a == 0)
          map.find(k);
        else if (a == 1)
          map[k] = v;
        else
          map.erase(k);
      }
      double time  = CycleTimer::currentSeconds() - start;
      results.push_back(time);
    }

    // Publish Results
    double best_time = *std::min_element(results.begin(), results.end());
    double avg_time  = std::accumulate(results.begin(), results.end(), 0.0) / static_cast<double>(results.size());
    std::cout << "\t" << "Max Throughput: " << m_op_count / best_time * 1000.0 << " ops/ms" << std::endl;
    std::cout << "\t" << "Avg Throughput: " << m_op_count / avg_time  * 1000.0 << " ops/ms" << std::endl;
}

void BenchmarkUnorderedMap::run()
{
  benchmark_all();
}

#endif