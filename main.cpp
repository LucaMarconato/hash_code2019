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

// int score_between_two_set_of_tags(set<int>& set1, set<int>& set2)
// {
//     set<int> intersection, diff1, diff2;
//     set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersection, intersection.begin()));
//     set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(diff1, diff1.begin()));
//     set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::inserter(diff2, diff2.begin()));
//     int score1, score2, score3;
//     score1 = intersection.size();
//     score2 = diff1.size();
//     score3 = diff2.size();
//     int score = 1000000;
//     score = min(score1, score2);
//     score = min(score, score3);
//     return score;
// }

// // map<pair<int, int>,
// int score_between_two_photos(int p1, int p2)
// {
//     set<int>& set1 = photos[p1].tags;
//     set<int>& set2 = photos[p2].tags;
//     int score = score_between_two_set_of_tags(set1, set2);
//     return score;
// }

// set<int>& get_tags_of_pair_of_vertical_photos(int v1, int v2)
// {
//     int vv1 = min(v1, v2);
//     int vv2 = max(v1, v2);
//     v1 = vv1;
//     v2 = vv2;
//     pair<int, int> p = make_pair(v1, v2);
//     if (tags_of_pairs_of_vertical_photos.find(p) == tags_of_pairs_of_vertical_photos.end()) {
//         set<int>& set1 = photos[v1].tags;
//         set<int>& set2 = photos[v2].tags;
//         set<int> the_union;
//         set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(the_union, the_union.begin()));
//         tags_of_pairs_of_vertical_photos[p] = the_union;
//     }
//     return tags_of_pairs_of_vertical_photos.at(p);
// }

// int score_between_a_photo_and_two_vertical(int p1, int v1, int v2)
// {
//     int vv1 = min(v1, v2);
//     int vv2 = max(v1, v2);
//     v1 = vv1;
//     v2 = vv2;
//     set<int>& set1 = photos[p1].tags;
//     set<int>& set2 = get_tags_of_pair_of_vertical_photos(v1, v2);
//     int score = score_between_two_set_of_tags(set1, set2);
//     return score;
// }

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
    void remove_at_index(iTerator idx)
    {
        this->total_niceness += this->score_of_removing_at_index(idx);
        data.erase(idx);
    }
    void insert_at_index(iTerator idx, frame* fr)
    {
        this->total_niceness += this->score_of_inserting_at_index(idx, fr);
        data.insert(idx, fr);
    }
    int score_of_inserting_at_index(iTerator idx, frame* fr)
    {
        bool inserting_at_the_beginning = idx == this->begin();
        bool inserting_at_the_end = idx == this->end();
        int delta_niceness = 0;
        if (data.size() > 0) {
            if (inserting_at_the_beginning) {
                delta_niceness += (*idx)->niceness(*fr);
            } else if (inserting_at_the_end) {
                auto prev = idx;
                prev--;
                delta_niceness += (*prev)->niceness(*fr);
            } else {
                auto prev = idx;
                prev--;
                delta_niceness += (*idx)->niceness(*fr);
                delta_niceness += (*prev)->niceness(*fr);
                delta_niceness -= (*prev)->niceness(*(*idx));
            }
        }
        return delta_niceness;
    }
    int score_of_removing_at_index(iTerator idx)
    {
        bool removing_at_the_beginning = idx == this->begin();
        auto penultimate = this->end();
        if (penultimate != this->begin()) {
            penultimate--;
        }
        bool removing_at_the_end = idx == penultimate;
        int delta_niceness = 0;
        if (data.size() > 2) {
            if (removing_at_the_beginning) {
                auto next = idx;
                next++;
                delta_niceness -= (*idx)->niceness(*(*next));
            } else if (removing_at_the_end) {
                auto prev = idx;
                prev--;
                delta_niceness += (*idx)->niceness(*(*prev));
            } else {
                auto next = idx;
                next++;
                auto prev = idx;
                prev++;
                delta_niceness -= (*prev)->niceness(*(*idx));
                delta_niceness -= (*idx)->niceness(*(*next));
                delta_niceness += (*prev)->niceness(*(*next));
            }
        } else {
            std::cerr << "error: you should never reach this point (case 0)\n";
            exit(1);
        }
        return delta_niceness;
    }
    int score_between_two_positions(iTerator idx1, iTerator idx2) { return static_cast<int>((*idx1)->niceness(*(*idx2))); }
    iTerator begin() { return data.begin(); }
    iTerator end() { return data.end(); }
    list<frame*> data;
    int total_niceness = 0;
};
//----------------------------------------------------------------------------------------------------

void test();
void local_search(slideshow& ss, bool do_not_remove);
void output(slideshow& ss);
void printing(slideshow& ss);
void printing_availables();
void remove_frame_from_available_list(frame* fr);
void add_frame_to_available_list(frame* fr);

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
    ss.insert_at_index(ss.begin(), &f1);
    ss.insert_at_index(ss.begin(), &f2);
    remove_frame_from_available_list(&f1);
    remove_frame_from_available_list(&f2);
    local_search(ss, false);
    // test();

    cout << "\n";
    return 0;
}

int best_score_found = -1;

void remove_frame_from_available_list(frame* fr)
{
    auto l = fr->images;
    for (auto e : l) {
        if (photos[e].vertical) {
            auto it = vertical_availables.find(e);
            vertical_availables.erase(it);
        } else {
            auto it = horizontal_availables.find(e);
            horizontal_availables.erase(it);
        }
    }
}

void add_frame_to_available_list(frame* fr)
{
    auto l = fr->images;
    if (l.size() == 0) {
        cerr << "this should never happen\n";
        exit(1);
    }
    for (auto e : l) {
        if (photos[e].vertical) {
            vertical_availables.insert(e);
        } else {
            horizontal_availables.insert(e);
        }
    }
}

void local_search(slideshow& ss, bool do_not_remove = false)
{
    while(1) {
        cout << "--begin local_search header"
             << "ss.size() = " << ss.size() << ", horizontal_availables.size() = " << horizontal_availables.size() << ", vertical_availables.size() = " << vertical_availables.size() << "\n";
        printing(ss);
        printing_availables();
        cout << "--end local_search header\n";
        xorshift();
        if (x % 3 == 0) {
            std::cout << "trying case 0\n";
            // insert a new horizontal picture or go to the next move
            if (horizontal_availables.size() > 0) {
                std::cout << "inside case 0\n";
                int best_photo_index = -1;
                slideshow::iTerator best_position_to_insert_at;
                int max_score = -100000;
                for (auto& photo_to_insert : horizontal_availables) {
                    for (auto it = ss.begin();; it++) {
                        frame f(photo_to_insert, -1);
                        int candidate_score = ss.score_of_inserting_at_index(it, &f); // DONE
                        std::cout << "candidate_score = " << candidate_score << "\n";
                        if (candidate_score > max_score) {
                            max_score = candidate_score;
                            best_position_to_insert_at = it;
                            best_photo_index = photo_to_insert;
                        }
                        if (it == ss.end()) {
                            break;
                        }
                    }
                }
                if (best_photo_index == -1) {
                    std::cerr << "error: you should never reach this point (case 1)\n";
                    exit(1);
                }
                frame f(best_photo_index, -1);
                ss.insert_at_index(best_position_to_insert_at, &f); // DONE
                remove_frame_from_available_list(&f);
                printing(ss);
                printing_availables();
            } else {
                x++;
            }
        }
        if (x % 3 == 1) {
            std::cout << "trying case 1\n";
            // insert a new pair of vertical pictures or go to the next move
            if (vertical_availables.size() >= 2) {
                std::cout << "inside case 1\n";
                int best_photo_index1 = -1;
                int best_photo_index2 = -1;
                slideshow::iTerator best_position_to_insert_at;
                int max_score = -100000;
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
                        for (auto it = ss.begin();; it++) {
                            int min_of_the_pair = min(photo_to_insert1, photo_to_insert2);
                            int max_of_the_pair = max(photo_to_insert1, photo_to_insert2);
                            frame f(min_of_the_pair, max_of_the_pair);
                            int candidate_score = ss.score_of_inserting_at_index(it, &f); // DONE
                            if (candidate_score > max_score) {
                                max_score = candidate_score;
                                best_position_to_insert_at = it;
                                best_photo_index1 = photo_to_insert1;
                                best_photo_index2 = photo_to_insert2;
                            }
                            if (it == ss.end()) {
                                break;
                            }
                        }
                        vertical_analyzed++;
                        if (vertical_analyzed > max_vertical_to_analyze) {
                            break;
                        }
                    }
                    if (vertical_analyzed > max_vertical_to_analyze) {
                        break;
                    }
                    j++;
                }
                int min_of_the_pair = min(best_photo_index1, best_photo_index2);
                int max_of_the_pair = max(best_photo_index1, best_photo_index2);
                if (min_of_the_pair == -1) {
                    std::cerr << "error: you should never reach this point (case 2)\n";
                    exit(1);
                }
                frame f(min_of_the_pair, max_of_the_pair);
                ss.insert_at_index(best_position_to_insert_at, &f); // DONE
                remove_frame_from_available_list(&f);
                printing(ss);
                printing_availables();
            } else {
                x++;
            }
        }
        if (x % 3 == 2) {
            std::cout << "trying case 2\n";
            if (do_not_remove) {
                local_search(ss, true);
                return;
            }
            // remove one picture and add another one
            int slideshow_size = ss.size(); // DONE
            if (slideshow_size >= 3) {
                std::cout << "inside case 2\n";
                int max_score = -100000;
                slideshow::iTerator best_index;
                auto it = ss.begin();
                for (; it != ss.end(); it++) {
                    int score = ss.score_of_removing_at_index(it);
                    std::cout << "score = " << score << "\n";
                    if (score > max_score) {
                        max_score = score;
                        best_index = it;
                    }
                }
                ss.remove_at_index(best_index); // DONE
                add_frame_to_available_list(*best_index);
                printing(ss);
                printing_availables();
                // here I add another one
                local_search(ss, true);
            } else {
                if (slideshow_size == 0) {
                    cerr << "error: slideshow_size = " << slideshow_size << "\n";
                }
            }
        }
        if (ss.total_niceness > best_score_found) {
            output(ss);
            best_score_found = ss.total_niceness;
        }
        // local_search(ss, false);
    }
}

void output(slideshow& ss)
{
    ofstream out("output.txt");
    for (auto it = ss.begin(); it != ss.end(); it++) {
        auto l = (*it)->images;
        for (auto e : l) {
            out << e << " ";
        }
        out << "\n";
    }
    out.close();
}

void printing(slideshow& ss)
{
    cout << "--begin printing ss--\n";
    for (auto jt = ss.begin(); jt != ss.end(); jt++) {
        auto l = (*jt)->images;
        for (auto e : l) {
            cout << e << " ";
        }
        cout << "\n";
    }
    cout << "--end printing ss--\n";
}

void printing_availables()
{
    cout << "horizonal available: ";
    for (auto e : horizontal_availables) {
        cout << e << " ";
    }
    cout << "\n";
    cout << "vertical available: ";
    for (auto e : vertical_availables) {
        cout << e << " ";
    }
    cout << "\n";
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
