

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
    string s = bit_string(a);
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
    printf("DiffBit: %d\n", __builtin_popcountl(a^b));
}

void Test_DoubleCheck() {
    const size_t Num = 30000000;
    size_t QueryCount = 20;
    int DiffBit = 7;

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
    dict.build(hashs, Num);
    SHOWDIFFTIME(build);

    // search.
    BEGIN;
    int current_count = 0;
    for (int i=0; i<QueryCount; ++i) {
        size_t query = random_i64();
        vector<size_t> search_ans = dict.search(query, DiffBit);
        sort(search_ans.begin(), search_ans.end());


        vector<size_t> raw_ans;
        for (int i=0; i<Num; ++i) {
            if (dict.diffbit(hashs[i], query) <= DiffBit) {
                raw_ans.push_back(hashs[i]);
            }
        }
        sort(raw_ans.begin(), raw_ans.end());

        fprintf(stderr, "query [%s] search_ans=%lu raw_ans=%lu\n", bit_string(query).c_str(), search_ans.size(), raw_ans.size());
        if (raw_ans.size() == search_ans.size()) {
            bool all_same = true;
            for (size_t i=0; i<raw_ans.size(); ++i) {
                if (raw_ans[i] != search_ans[i]) {
                    all_same = false;
                    break;
                }
            }
            if ( all_same ) {
                current_count += 1;
            }
        }
    }
    SHOWDIFFTIME(in_dict_search);

    fprintf(stderr, "query : %lu\n", QueryCount);
    fprintf(stderr, "current count : %d\n", current_count);

    delete [] hashs;
    return ;
}


void Test_3000w() {
    const size_t Num = 30000000;
    size_t QueryCount = 100000;

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
    dict.build(hashs, Num);
    SHOWDIFFTIME(build);

    // search.
    BEGIN;
    size_t ans_count = 0;
    size_t perf_count = 0;
    for (int i=0; i<QueryCount; ++i) {
        size_t query = hashs[random() % Num];
        vector<size_t> ans = dict.search(query, 7);
        ans_count += ans.size();
        //fprintf(stderr, "ans : %lu\n", ans.size());
        perf_count += dict.get_performance_counter();
    }
    SHOWDIFFTIME(in_dict_search);
    fprintf(stderr, "query : %lu\n", QueryCount);
    fprintf(stderr, "ans / query : %.2f\n", ans_count * 1.0 / QueryCount);
    fprintf(stderr, "perf / query : %.2f\n", perf_count * 1.0 / QueryCount);
    fprintf(stderr, "qps : %.2f\n", QueryCount * 1.0 / difft);

    delete [] hashs;
    return ;
}


int main() {

    //srand(19870609);
    srand((int)time(0));

    Test_DoubleCheck();

    //Test_DiffBit();

    //Test_3000w();
    return 0;
}
