

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "HashSearcher.h"


size_t bt, et, difft;
#define BEGIN  {bt = time(NULL);}
#define SHOWDIFFTIME(str) {et = time(NULL); fprintf(stderr, "Time [ " #str " ]: %ld\n", et - bt);}


void Test_3000w() {

    const size_t Num = 2000000;
    BEGIN;
    size_t *hashs = new size_t [Num];
    for (size_t i=0; i<Num; ++i) {
        size_t h = (size_t)rand();
        size_t l = (size_t)rand();
        hashs[i] = (h << 32) | l;
    }
    SHOWDIFFTIME(random);

    // build.
    BEGIN;
    HashSearcher dict;
    dict.build(hashs, Num, 5);
    SHOWDIFFTIME(build);

    // search.
    BEGIN;
    size_t ans_count = 0;
    size_t perf_count = 0;
    size_t query_count = 100;
    for (int i=0; i<query_count; ++i) {
        size_t query = hashs[random() % Num];
        vector<size_t> ans = dict.search(query);
        ans_count += ans.size();
        perf_count += dict.get_performance_counter();
    }
    fprintf(stderr, "query : %lu\n", query_count);
    fprintf(stderr, "ans / query : %.2f\n", ans_count * 1.0 / query_count);
    fprintf(stderr, "perf / query : %.2f\n", perf_count * 1.0 / query_count);

    SHOWDIFFTIME(in_dict_search);
    

    delete [] hashs;
    return ;
}


int main() {
    srand(19870609);

    Test_3000w();
    return 0;
}
