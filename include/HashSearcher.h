#ifndef __HASH_SEARCHER_H__
#define __HASH_SEARCHER_H__
/*
 * Hash Searcher header.
 */

#include <vector>
#include <algorithm>
#include <string>
using namespace std;

#include <unordered_set>
#include <unordered_map>

string bit_string(size_t a) {
    string s = "";
    for (int i=0; i<64; ++i) {
        if (a & 1) {
            s = "1" + s;
        } else {
            s = "0" + s;
        }
        a >>= 1;
    }
    return s;
}

typedef size_t HashType_t;
struct StoredItem_t{
    HashType_t code;
    size_t index;
};
typedef unordered_map<HashType_t, vector<StoredItem_t> > StoreType_t;

class HashSearcher {
    public:
        HashSearcher() {
            _current_max_index = 0;
            for (size_t i=0; i<4; ++i) {
                _masks.push_back( (0xFFFFLL << (i * 16LL)) );
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
            HashType_t x = a ^ b;
            return __db[ x & 0xFFFFLL ] + __db[ (x>>16) & 0xFFFFLL ] + __db[ (x>>32) & 0xFFFFLL] + __db[ (x>>48) & 0xFFFFLL ];
        };

        vector<HashType_t> search(HashType_t query, int diff_bit) {
            _performance_counter = 0;

            int max_min_bucket_diff = diff_bit / 4;
            unordered_set<HashType_t> ans_set;

            vector<HashType_t> buckets = get_buckets(query);
            for (size_t i=0; i<buckets.size(); ++i) {
                //fprintf(stderr, "try : %lu\n", buckets[i]);
                dfs_locate(query, buckets[i],  max_min_bucket_diff, diff_bit, &ans_set, i*16, (i+1)*16);
            }
            vector<HashType_t> ans;
            for (unordered_set<HashType_t>::iterator it=ans_set.begin(); it!=ans_set.end(); ++it) {
                ans.push_back(*it);
            }
            return ans;
        }

        void build(HashType_t* hashbuffer, size_t count) {
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
                    item.index = i;
                    _repo[buckets[i]].push_back(item);
                }
            }
            _current_max_index = count;
        }

        bool insert(HashType_t hash) {
            vector<HashType_t> buckets = get_buckets(hash);
            for (size_t i=0; i<buckets.size(); ++i) {
                StoredItem_t item;
                item.code = hash;
                item.index = _current_max_index;
                _repo[buckets[i]].push_back(item);
            }
            _current_max_index += 1;
            return true;
        }

        size_t get_performance_counter() const {return _performance_counter;}

    private:
        vector<HashType_t> get_buckets(HashType_t hash) {
            vector<HashType_t> buckets;
            for (size_t i=0; i<_masks.size(); ++i) {
                buckets.push_back(hash & _masks[i]);
            }
            return buckets;
        }

        void dfs_locate(HashType_t hash, HashType_t bucket, int bit_can_change, int diff, unordered_set<HashType_t>* pool, int begin_bit, int end_bit) {
            // no changes.
            //fprintf(stderr, "dfs: [%s] %d, %lu\n", bit_string(bucket).c_str(), bit_can_change, bucket);
            locate(hash, bucket, pool, diff);
            if (bit_can_change == 0) return;

            for (int i=begin_bit; i<end_bit; ++i) {
                size_t mask = (1LL << i);
                HashType_t new_bucket = bucket ^ mask;
                if (new_bucket != bucket) {
                    // one bit changes.
                    dfs_locate(hash, new_bucket, bit_can_change-1, diff-1, pool, begin_bit, end_bit);
                }
            }
        }

        void locate(HashType_t hash, HashType_t bucket, unordered_set<HashType_t>* pool, int threshold) {
            if (_repo.find(bucket) == _repo.end()) {
                return;
            }
            const vector<StoredItem_t>& l = _repo.find(bucket)->second;
            if (l.size() == 0) return ;

            for (size_t i=0; i<l.size(); ++i) {
                _performance_counter ++;
                int db = diffbit(l[i].code, hash);
                if (db <= threshold) {
                    pool->insert(l[i].code);
                }
            }
            return ;
        }

        StoreType_t _repo;
        vector<HashType_t> _masks;
        size_t _current_max_index;

        size_t _performance_counter;

        int __db[0xFFFF];
};

#endif



