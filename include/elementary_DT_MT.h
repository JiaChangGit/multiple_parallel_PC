#ifndef _ELEMENTARY_H_
#define _ELEMENTARY_H_
#include <getopt.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <cstdint>
// #include <cstdio>  // JIA remove printf
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "basis.hpp"

using namespace std;

struct CommandStruct {
  string run_mode;
  string method_name;

  string rules_file;
  string traces_file;
  string ans_file;
  string output_file;

  int rules_shuffle;
  int lookup_round;
  int force_test;
  int print_mode;

  int prefix_dims_num;

  int lookup_thread_time;
  int update_thread_speed;
  int reconstruct_thread_time;

  int next_layer_rules_num;

  void Init();
};

struct AccessNum {
  int sum;
  int minn;
  int maxn;
  int num;

  void Update() {
    sum += num;
    minn = min(minn, num);
    maxn = max(maxn, num);
  }

  void ClearNum() { num = 0; }

  void AddNum() { ++num; }
};

struct ProgramState {
  int rules_num;
  int traces_num;

  double data_memory_size;   // MB
  double index_memory_size;  // MB

  double build_time;    // S
  double lookup_speed;  // Mlps
  double insert_speed;  // Mups
  double delete_speed;  // Mups
  double update_speed;  // Mups

  int tuples_num;
  int tuples_sum;
  int hash_node_num;
  int bucket_sum;
  int bucket_use;
  int next_layer_num;

  AccessNum access_tuples;
  AccessNum access_tables;
  AccessNum access_nodes;
  AccessNum access_rules;

  int max_access_all;
  int max_access_tuple_node_rule;
  int max_access_node_rule;

  int low_priority_matching_access;
  int high_priority_matching_access;

  int low_priority_collision_access;
  int high_priority_collision_access;

  int low_priority_rule_access;
  int high_priority_rule_access;

  double dt_time;  // S

  void AccessClear();
  void AccessCal();
};

struct Rule_DT_MT {
  uint32_t range[5][2];
  char prefix_len[5];
  int priority;

  bool operator<(const Rule_DT_MT &rule) const {
    for (int i = 0; i < 5; ++i)
      for (int j = 0; j < 2; ++j)
        if (range[i][j] != rule.range[i][j])
          return range[i][j] < rule.range[i][j];
    for (int i = 0; i < 5; ++i)
      if (prefix_len[i] != rule.prefix_len[i])
        return prefix_len[i] < rule.prefix_len[i];
    return priority < rule.priority;
  }

  bool operator==(const Rule_DT_MT &rule) const {
    for (int i = 0; i < 5; ++i)
      for (int j = 0; j < 2; ++j)
        if (range[i][j] != rule.range[i][j]) return false;
    for (int i = 0; i < 5; ++i)
      if (prefix_len[i] != rule.prefix_len[i]) return false;
    return priority == rule.priority;
  }
};

struct Trace {
  union {
    uint32_t key[5];
    uint64_t dst_src_ip;
  };
};

class Classifier {
 public:
  virtual int Create(vector<Rule_DT_MT *> &rules, bool insert) = 0;

  virtual int InsertRule(Rule_DT_MT *rule) = 0;
  virtual int DeleteRule(Rule_DT_MT *rule) = 0;
  virtual int Lookup(Trace *trace, int priority) = 0;
  virtual int LookupAccess(Trace *trace, int priority, Rule_DT_MT *ans_rule,
                           ProgramState *program_state) = 0;

  virtual int Reconstruct() = 0;
  virtual uint64_t MemorySize() = 0;
  virtual int CalculateState(ProgramState *program_state) = 0;
  virtual int GetRules(vector<Rule_DT_MT *> &rules) = 0;
  virtual int Free(bool free_self) = 0;
  virtual int Test(void *ptr) = 0;

  int test_num;
  pthread_mutex_t lookup_mutex;
  pthread_mutex_t update_mutex;
};

bool SameRule(Rule_DT_MT *rule1, Rule_DT_MT *rule2);
bool CmpRulePriority(Rule_DT_MT *rule1, Rule_DT_MT *rule2);
uint64_t GetRunTimeUs(timeval timeval_start, timeval timeval_end);
uint64_t GetAvgTime(vector<uint64_t> &lookup_times);

#endif
