#include <iostream>
#include <string>
#include <vector>
using namespace std;

// 构建 next 数组
vector<int> getNext(const string& pattern) {
    vector<int> next(pattern.size());
    next[0] = 0;
    int len = 0;
    int i = 1;
    
    while (i < pattern.size()) {
        if (pattern[i] == pattern[len]) {
            len++;
            next[i] = len;
            i++;
        } else {
            if (len == 0) {
                next[i] = 0;
                i++;
            } else {
                len = next[len - 1];
            }
        }
    }
    return next;
}

// 查找模式串在文本串中第一次出现的位置
int kmp(const string& text, const string& pattern) {
    if (pattern.empty()) return 0;
    if (pattern.size() > text.size()) return -1;

    vector<int> next = getNext(pattern);
    int i = 0, j = 0;

    while (i < text.size()) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        } else if (j > 0) {
            j = next[j - 1];
        } else {
            i++;
        }

        if (j == pattern.size()) {
            return i - j;
        }
    }
    return -1;
}

// 查找所有匹配位置
vector<int> kmpFindAll(const string& text, const string& pattern) {
    vector<int> positions;
    if (pattern.empty() || pattern.size() > text.size()) return positions;

    vector<int> next = getNext(pattern);
    int i = 0, j = 0;

    while (i < text.size()) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        } else if (j > 0) {
            j = next[j - 1];
        } else {
            i++;
        }

        if (j == pattern.size()) {
            positions.push_back(i - j);
            j = next[j - 1];
        }
    }
    return positions;
}

int main() {
    // 测试1：查找单个匹配
    string text = "ABABCABCABABABD";
    string pattern = "ABABD";
    int pos = kmp(text, pattern);
    cout << "First match position: " << pos << endl;

    // 测试2：查找所有匹配
    string text2 = "ABABABABAB";
    string pattern2 = "ABA";
    vector<int> all_pos = kmpFindAll(text2, pattern2);
    cout << "\nAll matches:" << endl;
    for (int p : all_pos) {
        cout << "Found at: " << p << endl;
    }

    return 0;
}