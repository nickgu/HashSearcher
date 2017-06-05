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
struct StoredItem_t{
    HashType_t code;
    int diff_bit;

    bool operator < (const StoredItem_t& b) const {
        return diff_bit < b.diff_bit;
    }
};
typedef unordered_map<HashType_t, vector<StoredItem_t> > StoreType_t;

class HashSearcher {
    public:
        HashSearcher() {
            for (size_t i=0; i<8; ++i) {
                _masks.push_back( (0xFFLL << (i * 8LL)) );
            }

            for (size_t i=0; i<0xFFFFLL; ++i) {
                __db[i] = 0;
                size_t test = i;
                while (test) {
                    if (test & 0x1) __db[i] ++;
                    test >>= 1;
                }
            }
        }

        int diffbit(HashType_t a, HashType_t b) {
            int r = 0;
            HashType_t x = a ^ b;
            while (x) {
                r+=__db[ x & 0xFFFFLL ];
                x >>= 16;
            }
            return r;
        };

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
                    StoredItem_t item;
                    item.code = hash;
                    item.diff_bit = 0;
                    if (_repo[buckets[i]].size() > 0) {
                        item.diff_bit = diffbit(_repo[buckets[i]][0].code, hash);
                    }
                    _repo[buckets[i]].push_back(item);
                }
            }

            fprintf(stderr, "begin reorder. buckets.size=%lu\n", _repo.size());
            for (StoreType_t::iterator it = _repo.begin(); it != _repo.end(); ++it) {
                sort(it->second.begin(), it->second.end());

                // debug list.
                /*
                vector<StoredItem_t>& v = it->second;
                fprintf(stderr, "mask [%lu] : ", it->first);
                for (size_t i=0; i<v.size(); ++i) {
                    if (i==0 || v[i].diff_bit!=v[i-1].diff_bit) {
                        fprintf(stderr, "(%lu:%d)", i, v[i].diff_bit);
                    }
                }
                fprintf(stderr, "  total:%lu\n", v.size());
                */
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
            const vector<StoredItem_t>& l = _repo.find(bucket)->second;
            if (l.size() == 0) return ;

            int db = diffbit(hash, l[0].code);

            // add bi-search later.
            StoredItem_t lower, upper;
            lower.diff_bit = db - _diff_bit;
            upper.diff_bit = db + _diff_bit;

            vector<StoredItem_t>::const_iterator beg = lower_bound(l.begin(), l.end(), lower);
            vector<StoredItem_t>::const_iterator end = upper_bound(l.begin(), l.end(), upper);   

            //fprintf(stderr, "vector_size : %lu\n", l.size());
            //fprintf(stderr, "bisearch_size : %ld\n", end - beg);

            for (vector<StoredItem_t>::const_iterator it=beg; it != end; it++) {
                _performance_counter ++;
                if (diffbit(it->code, hash) <= _diff_bit) {
                    pool->insert(it->code);
                }
            }
            return ;
        }

        int _diff_bit;
        StoreType_t _repo;
        vector<HashType_t> _masks;
        size_t _performance_counter;

        int __db[0xFFFF];
};

#endif



