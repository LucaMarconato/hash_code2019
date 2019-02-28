#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

typedef unsigned short d; //distance
typedef int pi;        //photo index 
typedef int ti;

struct frame {
    frame(pi p1, pi p2){
        set_union(pictures[p1].tags.begin(), pictures[p1].tags.end(),pictures[p2].tags.begin(), pictures[p2].tags.end(),inserter(tags,tags.begin()));
    }
    
    d niceness(frame& other) {
        set<ti> tmp;
        set_intersection(tags.begin(),tags.end(),other.tags.begin(),other.tags.end(),tmp.begin());
        int m=tmp.count();
        tmp.clear();
        set_difference(tags.begin(),tags.end(),other.tags.begin(),other.tags.end(),tmp.begin());
        m=min(m,tmp.count());
        tmp.clear();
        set_difference(other.tags.begin(),other.tags.end(),tags.begin(),tags.end(),tmp.begin());
        m=min(m,tmp.count());
        return m;
    }
    
    set<ti> tags; 
    list<pi> images;
};

struct slideshow
{
    typedef iTerator list<frame*>::iterator;
    int size(){data.size();}
    void remove_at_index(iTerator idx){data.erase(idx);}
    void insert_after_index(iTerator idx, frame* fr){data.insert(idx,fr);}
    int score_of_inserting_after_index(iTerator idx, frame* fr){
        auto next=idx;
        next++;
        return static_cast<int>((*idx)->niceness(*(*fr)))-static_cast<int>((*idx)->niceness(*(*next)));
    }
    int score_between_two_positions(iTerator idx){
        auto next=idx;
        next++;
        return static_cast<int>((*idx)->niceness(*(*next)));
    }
    
    list<frame*> data;
}
    
map<string, int> tags_dictionary;
struct photo {
    bool vertical;
    set<int> tags;
};
int added_tags = 0;

map<pair<int, int>, set<int>> tags_of_pairs_of_vertical_photos;

photo photos[100000];

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

int main(int argc, char* argv[])
{
    string filename = argv[1] + (string) "_example.txt";
    ifstream in(filename);
    int lines;
    in >> lines;
    for (int i = 0; i < lines; i++) {
        char orientation;
        in >> orientation;
        photos[i].vertical = orientation == 'V';
        int tags_count;
        in >> tags_count;
        string tag;
        for (int j = 0; j < tags_count; j++) {
            in >> tag;
            if (tags_dictionary.find(tag) == tags_dictionary.end()) {
                tags_dictionary[tag] = added_tags;
                photos[i].tags.insert(added_tags);
                added_tags++;
            } else {
                int i_tag = tags_dictionary.at(tag);
                photos[i].tags.insert(i_tag);
            }
        }
    }
    in.close();
    cout << "\n";
    return 0;
}
