#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

//------------------------------FAST RANDOM NUMBER GENERATOR
static int x;

int xorshift()
{
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

void xorshift_init()
{
    x = time(NULL);
    xorshift();
    xorshift();
    xorshift();
}

//------------------------------

map<string, int> tags_dictionary;
struct photo {
    bool vertical;
    set<int> tags;
};
int total_photos = 0;
int total_tags = 0;
map<pair<int, int>, set<int>> tags_of_pairs_of_vertical_photos;
photo photos[100000];
set<int> vertical_availables;
set<int> horizontal_availables;

int score_between_two_set_of_tags(set<int>& set1, set<int>& set2)
{
    set<int> intersection, diff1, diff2;
    set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersection, intersection.begin()));
    set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(diff1, diff1.begin()));
    set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::inserter(diff2, diff2.begin()));
    int score1, score2, score3;
    score1 = intersection.size();
    score2 = diff1.size();
    score3 = diff2.size();
    int score = 1000000;
    score = min(score1, score2);
    score = min(score, score3);
    return score;
}

// map<pair<int, int>,
int score_between_two_photos(int p1, int p2)
{
    set<int>& set1 = photos[p1].tags;
    set<int>& set2 = photos[p2].tags;
    int score = score_between_two_set_of_tags(set1, set2);
    return score;
}

set<int>& get_tags_of_pair_of_vertical_photos(int v1, int v2)
{
    int vv1 = min(v1, v2);
    int vv2 = max(v1, v2);
    v1 = vv1;
    v2 = vv2;
    pair<int, int> p = make_pair(v1, v2);
    if (tags_of_pairs_of_vertical_photos.find(p) == tags_of_pairs_of_vertical_photos.end()) {
        set<int>& set1 = photos[v1].tags;
        set<int>& set2 = photos[v2].tags;
        set<int> the_union;
        set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(the_union, the_union.begin()));
        tags_of_pairs_of_vertical_photos[p] = the_union;
    }
    return tags_of_pairs_of_vertical_photos.at(p);
}

int score_between_a_photo_and_two_vertical(int p1, int v1, int v2)
{
    int vv1 = min(v1, v2);
    int vv2 = max(v1, v2);
    v1 = vv1;
    v2 = vv2;
    set<int>& set1 = photos[p1].tags;
    set<int>& set2 = get_tags_of_pair_of_vertical_photos(v1, v2);
    int score = score_between_two_set_of_tags(set1, set2);
    return score;
}

//----------------------------------------------------------------------------------------------------
typedef unsigned short d; // distance
typedef int pi;           // photo index
typedef int ti;

struct frame {
    frame(pi p1, pi p2)
    {
        if (p2 == -1) {
            tags = photos[p1].tags;
            images.push_front(p1);
        } else {
            set_union(photos[p1].tags.begin(), photos[p1].tags.end(), photos[p2].tags.begin(), photos[p2].tags.end(), inserter(tags, tags.begin()));
            images.push_front(p1);
            images.push_front(p2);
        }
    }
    ~frame()
    {
        tags.clear();
        images.clear();
    }
    d niceness(frame& other)
    {
        set<ti> tmp;
        set_intersection(tags.begin(), tags.end(), other.tags.begin(), other.tags.end(), std::inserter(tmp, tmp.begin()));
        size_t m = tmp.size();
        tmp.clear();
        set_difference(tags.begin(), tags.end(), other.tags.begin(), other.tags.end(), std::inserter(tmp, tmp.begin()));
        m = min(m, tmp.size());
        tmp.clear();
        set_difference(other.tags.begin(), other.tags.end(), tags.begin(), tags.end(), std::inserter(tmp, tmp.begin()));
        m = min(m, tmp.size());
        return m;
    }

    set<ti> tags;
    list<pi> images;
};

struct slideshow {
    typedef list<frame*>::iterator iTerator;
    int size() { return data.size(); }
    void remove_at_index(iTerator idx) { data.erase(idx); }
    void insert_after_index(iTerator idx, frame* fr) { data.insert(idx, fr); }
    int score_of_inserting_after_index(iTerator idx, frame* fr)
    {
        auto next = idx;
        next++;
        return static_cast<int>((*idx)->niceness(*fr)) - static_cast<int>((*idx)->niceness(*(*next)));
    }
    int score_between_two_positions(iTerator idx)
    {
        auto next = idx;
        next++;
        return static_cast<int>((*idx)->niceness(*(*next)));
    }
    iTerator begin() { return data.begin(); }
    iTerator end() { return data.end(); }
    list<frame*> data;
};
//----------------------------------------------------------------------------------------------------

void test();
void local_search(slideshow& ss, bool do_not_remove);

int main(int argc, char* argv[])
{
    xorshift_init();
    string filename = argv[1] + (string) "_example.txt";
    ifstream in(filename);
    int lines;
    in >> lines;
    for (; total_photos < lines; total_photos++) {
        char orientation;
        in >> orientation;
        photos[total_photos].vertical = orientation == 'V';
        if (photos[total_photos].vertical) {
            vertical_availables.insert(total_photos);
        } else {
            horizontal_availables.insert(total_photos);
        }
        int tags_count;
        in >> tags_count;
        string tag;
        for (int j = 0; j < tags_count; j++) {
            in >> tag;
            if (tags_dictionary.find(tag) == tags_dictionary.end()) {
                tags_dictionary[tag] = total_tags;
                photos[total_photos].tags.insert(total_tags);
                total_tags++;
            } else {
                int i_tag = tags_dictionary.at(tag);
                photos[total_photos].tags.insert(i_tag);
            }
        }
    }
    in.close();

    frame f1(0, -1);
    frame f2(1, -1);
    slideshow ss;
    ss.insert_after_index(ss.begin(), &f1);
    ss.insert_after_index(ss.begin(), &f2);
    local_search(ss, false);
    // test();

    cout << "\n";
    return 0;
}

void local_search(slideshow& ss, bool do_not_remove = false)
{
    xorshift();
    if (x % 3 == 0) {
        // insert a new horizontal picture or go to the next move
        if (horizontal_availables.size() > 0) {
            int slideshow_size = ss.size(); // DONE
            int best_photo_index = -1;
            slideshow::iTerator best_position_to_insert_after;
            int best_score = -1;
            for (auto& photo_to_insert : horizontal_availables) {
                int i = 0;
                for (auto it = ss.begin(); it != ss.end(); it++, i++) {
                    if (i == slideshow_size - 1) {
                        continue;
                    }
                    frame f(photo_to_insert, -1);
                    int candidate_score = ss.score_of_inserting_after_index(it, &f); // DONE
                    if (candidate_score > best_score) {
                        best_score = candidate_score;
                        best_position_to_insert_after = it;
                        best_photo_index = photo_to_insert;
                    }
                }
            }
            frame f(best_photo_index, -1);
            ss.insert_after_index(best_position_to_insert_after, &f); // DONE
            auto it = horizontal_availables.find(best_photo_index);
            horizontal_availables.erase(it);
        } else {
            x++;
        }
    }
    if (x % 3 == 1) {
        // insert a new pair of vertical pictures or go to the next move
        if (vertical_availables.size() > 2) {
            int slideshow_size = ss.size(); // DONE
            int best_photo_index1 = -1;
            int best_photo_index2 = -1;
            slideshow::iTerator best_position_to_insert_after;
            int best_score = -1;
            int max_vertical_to_analyze = 1000;
            int vertical_analyzed = 0;
            xorshift();
            int random_vertical = x % vertical_availables.size();
            int j = 0;
            for (auto& photo_to_insert1 : vertical_availables) {
                if (j < random_vertical) {
                    j++;
                    continue;
                }
                for (auto& photo_to_insert2 : vertical_availables) {
                    if (photo_to_insert1 == photo_to_insert2) {
                        continue;
                    }
                    int i = 0;
                    for (auto it = ss.begin(); it != ss.end(); it++, i++) {
                        if (i == slideshow_size - 1) {
                            continue;
                        }
                        int min_of_the_pair = min(photo_to_insert1, photo_to_insert2);
                        int max_of_the_pair = max(photo_to_insert1, photo_to_insert2);
                        frame f(min_of_the_pair, max_of_the_pair);
                        int candidate_score = ss.score_of_inserting_after_index(it, &f); // DONE
                        if (candidate_score > best_score) {
                            best_score = candidate_score;
                            best_position_to_insert_after = it;
                            best_photo_index1 = photo_to_insert1;
                            best_photo_index2 = photo_to_insert2;
                        }
                    }
                    vertical_analyzed++;
                }
                j++;
            }
            int min_of_the_pair = min(best_photo_index1, best_photo_index2);
            int max_of_the_pair = max(best_photo_index1, best_photo_index2);
            frame f(min_of_the_pair, max_of_the_pair);
            ss.insert_after_index(best_position_to_insert_after, &f); // DONE
            auto it1 = vertical_availables.find(best_photo_index1);
            auto it2 = vertical_availables.find(best_photo_index2);
            vertical_availables.erase(it1);
            vertical_availables.erase(it2);
        } else {
            x++;
        }
    }
    if (x % 3 == 2) {
        if (do_not_remove) {
            local_search(ss, do_not_remove = true);
        }
        // remove one picture and add another one
        // here I remove one picture
        int slideshow_size = ss.size(); // DONE
        int min_score = 100000;
        int min_position = -1;
        int i = 0;
        auto it = ss.begin();
        for (; it != ss.end(); it++, i++) {
            if (i == 0 || i == slideshow_size - 1) {
                continue;
            }
            auto pred_it = it;
            auto succ_it = it;
            pred_it--;
            int score = ss.score_between_two_positions(pred_it) + ss.score_between_two_positions(it);
            if (score < min_score) {
                min_score = score;
                min_position = i;
            }
        }
        ss.remove_at_index(it); // DONE
        if (photos[i].vertical) {
            vertical_availables.insert(i);
        } else {
            horizontal_availables.insert(i);
        }
        // here I add another one
        local_search(ss, true);
    }
    local_search(ss, false);
}

void test()
{
//     for (int i = 0; i < 4; i++) {
//         for (int j = 0; j < 4; j++) {
//             std::cout << score_between_two_photos(i, j) << " ";
//         }
//         std::cout << score_between_a_photo_and_two_vertical(i, 0, 3) << " ";
//         std::cout << "\n";
//     }
//     for (int j = 0; j < 4; j++) {
//         std::cout << score_between_a_photo_and_two_vertical(j, 0, 3) << " ";
//     }
//     std::cout << score_between_two_photos(2, 3) << "\n";
//     std::cout << score_between_a_photo_and_two_vertical(1, 0, 3) << "\n";
//     cout << "total_tags = " << total_tags << "\n";
}
