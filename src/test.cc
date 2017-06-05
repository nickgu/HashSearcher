

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "HashSearcher.h"
#include <string>
using namespace std;

size_t bt, et, difft;
#define BEGIN  {bt = time(NULL);}
#define SHOWDIFFTIME(str) {et = time(NULL); difft=et-bt; fprintf(stderr, "Time [ " #str " ]: %ld\n", difft);}

size_t random_i64() {
    size_t h1 = (size_t)(rand() & 0xFFFFLL);
    size_t h2 = (size_t)(rand() & 0xFFFFLL) << 16;
    size_t h3 = (size_t)(rand() & 0xFFFFLL) << 32;
    size_t h4 = (size_t)(rand() & 0xFFFFLL) << 48;


    // this is for random.
    return h1 | h2 | h3 | h4;
}

void debug_show_i64(size_t a) {
    string s = "";
    for (int i=0; i<64; ++i) {
        if (a & 1) {
            s = "1" + s;
        } else {
            s = "0" + s;
        }
        a >>= 1;
    }
    fprintf(stderr, "%s\n", s.c_str());
    return ;
}

void Test_DiffBit() {
    HashSearcher dict;

    size_t a = random_i64();
    size_t b = random_i64();

    debug_show_i64(a);
    debug_show_i64(b);
    debug_show_i64(a ^ b);

    printf("DiffBit: %d\n", dict.diffbit(a,b));
}

void Test_3000w() {

    const size_t Num = 30000000;
    BEGIN;
    size_t *hashs = new size_t [Num];
    for (size_t i=0; i<Num; ++i) {
        // this is compact code.
        //hashs[i] = (size_t) i;
        hashs[i] = random_i64();
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
    size_t query_count = 2000;
    for (int i=0; i<query_count; ++i) {
        size_t query = hashs[random() % Num];
        vector<size_t> ans = dict.search(query);
        ans_count += ans.size();
        //fprintf(stderr, "ans : %lu\n", ans.size());
        perf_count += dict.get_performance_counter();
    }
    fprintf(stderr, "query : %lu\n", query_count);
    fprintf(stderr, "ans / query : %.2f\n", ans_count * 1.0 / query_count);
    fprintf(stderr, "perf / query : %.2f\n", perf_count * 1.0 / query_count);
    fprintf(stderr, "qps : %.2f\n", query_count * 1.0 / difft);

    SHOWDIFFTIME(in_dict_search);
    

    delete [] hashs;
    return ;
}


int main() {
    //srand(19870609);
    srand((int)time(0));

    //Test_DiffBit();

    Test_3000w();
    return 0;
}
