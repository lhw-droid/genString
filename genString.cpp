#include <random>
#include <iostream>
#include <string>
#include <Windows.h>
#include <limits>
#include <vector>

#ifdef max
#undef max
#endif

using namespace std;

random_device rd;

namespace genRandString {
    using choice = unsigned int;

    constexpr choice
        cap =   0b00000001,
        low =   0b00000010,
        num =   0b00000100,
        sign =  0b00001000;
    string genString(int len = 16, choice c = cap | low | num | sign);
    using charPool = std::string;
    const charPool
        caPool = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        lowPool = "abcdefghijklmnopqrstuvwxyz",
        numPool = "0123456789",
        signPool = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
    struct Option {
        string name;
        choice flag;
        string pool;
    };
    const vector<Option> options = {
    {"capital", cap, caPool},
    {"lower",   low, lowPool},
    {"number",  num, numPool},
    {"signal",  sign, signPool}
    };
}

int main() {
    string ret;
    while (1) {
        int len;
        cout << "len: ";
        if (!(cin >> len))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (len <= 0) {
            cout << "Are you kidding?\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        genRandString::choice c = 0;
        for (size_t i = 0; i < genRandString::options.size(); i++) {
            string tmp;
            cout << genRandString::options[i].name << "? Default for yes.";
            getline(cin, tmp);
            if (tmp == "") {
                c |= genRandString::options[i].flag;
            }
        }
        ret = genRandString::genString(len, c);
        if (ret == "") {
            continue;
        }
        cout << ret << endl;
        if (OpenClipboard(NULL)) {
            HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, ret.size() + 1);
            if (hg) {
                if (void* pm = GlobalLock(hg)) {
                    memcpy(pm, ret.c_str(), ret.size() + 1);
                    GlobalUnlock(hg);
                    EmptyClipboard();
                    if (SetClipboardData(CF_TEXT, hg)) {
                        hg = nullptr; // ownership transferred
                        //see SetClipboardData at https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setclipboarddata
                        //If SetClipboardData succeeds, the system owns the object identified by the hMem parameter. The application may not write to or free the data once ownership has been transferred to the system
                    }
                    else {
                        cout << "Fail to set clipboard data, you may need to copy it yourself:(" << endl;
                    }
                }
                else {
                    cout << "Fail to lock memory, you may need to copy it yourself:(" << endl;
                }
            }
            else {
                cout << "Fail to allocate memory, you may need to copy it yourself:(" << endl;
            }
            if (hg)
            {
                GlobalFree(hg);
            }
            CloseClipboard();
        }
        else {
            cout << "Fail to open clipboard, you may need to copy it yourself:(" << endl;
        }
        cout << "Another?" << endl;
        getline(cin, ret);
        if (ret == "0" || ret == "") {
            break;
        }
    }
}

string genRandString::genString(int len, genRandString::choice c) {
    if (c == 0) {
        cout << "At least one category must be selected.\n";
        return "";
    }
    string genPool, ret;
    for (size_t i = 0; i < options.size(); i++) {
        if (c & options.at(i).flag) {
            genPool += options.at(i).pool;
        }
    }
    uniform_int_distribution<size_t> d(0, genPool.size() - 1);
    while (ret.size() < len) {
        ret += genPool.at(d(rd));
    }
    return ret;
}
