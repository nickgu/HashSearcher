#ifndef __HASH_SEARCHER_H__
#define __HASH_SEARCHER_H__
/*
 * Hash Searcher header.
 */

#include <vector>
#include <algorithm>
using namespace std;

#include <unordered_set>
#include <unordered_map>
//using namespace __gnu_cxx;

typedef size_t HashType_t;
typedef unordered_map<HashType_t, vector<HashType_t> > StoreType_t;

inline int diffbit(HashType_t a, HashType_t b) {
    int r = 0;
    HashType_t x = a ^ b;
    while (x) {
        if (x & 0x1) r++;
        x >>= 1;
    }
    return r;
};

class HashSearcher {
    public:
        HashSearcher() {
            for (size_t i=0; i<8; ++i) {
                _masks.push_back( (0xFFLL << (i * 8LL)) );
            }
        }

        vector<HashType_t> search(HashType_t query) {
            _performance_counter = 0;
            vector<HashType_t> buckets = get_buckets(query);
            unordered_set<HashType_t> ans_set;
            for (size_t i=0; i<buckets.size(); ++i) {
                locate( query, buckets[i], &ans_set );
            }
            vector<HashType_t> ans;
            for (unordered_set<HashType_t>::iterator it=ans_set.begin(); it!=ans_set.end(); ++it) {
                ans.push_back(*it);
            }
            return ans;
        }

        void build(HashType_t* hashbuffer, size_t count, int diff_bit) {
            _diff_bit = diff_bit;
            _repo.clear();

            for (size_t i=0; i<count; ++i) {
                if (i % 1000000 ==0) {
                    fprintf(stderr, "%lu\n", i);
                }
                HashType_t hash = hashbuffer[i];
                vector<HashType_t> buckets = get_buckets(hash);
                for (size_t i=0; i<buckets.size(); ++i) {
                    _repo[buckets[i]].push_back(hash);
                }
            }

            fprintf(stderr, "begin reorder. buckets.size=%lu\n", _repo.size());
            for (StoreType_t::iterator it = _repo.begin(); it != _repo.end(); ++it) {
                //sort(it->second.begin(), it->second.end(), DiffBitComparer_t(it->second[0]));
                sort(it->second.begin(), it->second.end());
            }
        }

        bool insert(HashType_t new_item);

        size_t get_performance_counter() const {return _performance_counter;}

    private:
        vector<HashType_t> get_buckets(HashType_t hash) {
            vector<HashType_t> buckets;
            for (size_t i=0; i<_masks.size(); ++i) {
                buckets.push_back(hash & _masks[i]);
            }
            return buckets;
        }

        void locate(HashType_t hash, HashType_t bucket, unordered_set<HashType_t>* pool) {
            const vector<HashType_t>& l = _repo.find(bucket)->second;
            if (l.size() == 0) return ;

            int db = diffbit(hash, l[0]);

            // add bi-search later.
            //int db_begin = db - _diff_bit;
            //int db_end = db + diff_bit;

            for (size_t i=0; i<l.size(); ++i) {
                _performance_counter ++;
                if (diffbit(l[i], hash) <= _diff_bit) {
                    pool->insert(l[i]);
                }
            }
            return ;
        }

        int _diff_bit;
        StoreType_t _repo;
        vector<HashType_t> _masks;
        size_t _performance_counter;

        struct DiffBitComparer_t {
            DiffBitComparer_t(HashType_t base) {
                _base = base;
            }

            bool operator () (HashType_t a, HashType_t b) const {
                int dba = diffbit(a, _base);
                int dbb = diffbit(b, _base);
                return dba < dbb;
            }

            HashType_t _base;
        };
};

#endif



