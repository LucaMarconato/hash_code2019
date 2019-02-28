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

map<string, int> tags_dictionary;
struct photo {
    bool vertical;
    set<int> tags;
};
int added_tags = 0;

photo photos[100000];

int main(int argc, char* argv[])
{
    string filename = argv[1] + (string) "_example.txt";
    ifstream in(filename);
    int lines;
    in >> lines;
    // cout << "lines = " << lines << "\n";
    for (int i = 0; i < lines; i++) {
        char orientation;
        in >> orientation;
        photos[i].vertical = orientation == 'V';
        // cout << "orientation = " << orientation << "\n";
        int tags_count;
        in >> tags_count;
        // cout << "tags_count = " << tags_count << "\n";
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
    // cout << "added_tags = " << added_tags << "\n";
    // cout << photos[0].vertical;
    // for (auto& e : photos[90].tags) {
    //     cout << e << " ";
    // }
    cout << "\n";
    return 0;
}
