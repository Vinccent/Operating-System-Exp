#include <bits/stdc++.h>

using namespace std;

//file systrm beta 1.1

struct fileNode {
    string name;
    string owner;
    string group;
    struct tm* currentTime;
    int size;
    int authority[3];

    fileNode(string tname = "") {
        name = tname;
        owner = "lz";
        group = "lz";
        const time_t t = time(NULL);
        currentTime = localtime(&t);
        size = sizeof(fileNode);
        for (int i = 0; i < 3; i++) authority[i] = 7;
    }
    
};

struct dirNode {
    string name;
    string owner; 
    dirNode* pa;
    string pwd;
    string group; //所属组
    struct tm* currentTime; // the time created
    int size; //the size of dir
    int authority[3]; //权限，三位10进制数表示

    //子目录
    set<dirNode*> sons;
    map<string, dirNode*> mm; //建立映射关系

    //子文件
    set<fileNode*> files;

    dirNode(string tname = "") {
        name = tname;
        owner = "lz";
        pa = NULL;
        pwd = "/";
        group = "lz";
        time_t t = time(NULL);
        currentTime = localtime(&t);
        size = sizeof(dirNode);
        for (int i = 0; i < 3; i++) authority[i] = 7;

        sons.clear();
        mm.clear();
        files.clear();

        mm["."] = this;
        mm[".."] = this;
    }

    dirNode(string tname, dirNode* f) {
        name = tname;
        owner = "lz";
        pa = f;
        pwd = f->pwd + tname + "/";
        group = "lz";
        time_t t = time(NULL);
        currentTime = localtime(&t);
        size = sizeof(dirNode);
        for (int i = 0; i < 3; i++) authority[i] = 7;

        sons.clear();
        mm.clear();
        files.clear();

    }
};

dirNode* root;
dirNode* cur;

void init() {
    root = new dirNode("root");
    cur = root;
}

queue<string> get_path_queue(string path) {
    queue<string> sonPath;
    int sz = path.size();
    if (path[sz - 1] != '/') path[sz++] = '/';
    vector<int> tmp;
    int sta = 1;
    for (int i = 0; i < sz; i++) {
        if (path[i] == '/') tmp.push_back(i);
    } 
    auto it = tmp.begin();
    it++;
    while(1) {
        if (it >= tmp.end()) break;
        string path_son = path.substr(sta, *it - sta);
        sta = *it;
        sta++;
        it++;
        sonPath.push(path_son);
    }  
    return sonPath;
}

void absolute_path_handler(string path) {
    queue<string> pathQueue = get_path_queue(path);
    dirNode* go = root;
    while (!pathQueue.empty()) {
        string name = pathQueue.front(); pathQueue.pop();
        if (go->mm.find(name) == go->mm.end()) {
            cout << "No such directory: " << path << endl;
            return;
        } else {
            dirNode* tmp = go->mm[name];
            go = tmp;
        }
    }
    cur = go;
}

void relative_path_handler(string path) {
    queue<string> pathQueue = get_path_queue("/" + path);
    while (!pathQueue.empty()) {
        string name = pathQueue.front(); pathQueue.pop();
        if (name == ".") {
            continue;
        } 
        if (name == "..") {
            if (cur == root) continue;
            cur = cur->pa;
            continue;
        }
        if (cur->mm.find(name) == cur->mm.end()) {
            cout << "No such directory: " << path << endl;
            return;
        } else {
            cur = cur->mm[name];
        }
    }
}

void cd_handler(string path) {
    if (path[0] == '/') absolute_path_handler(path);
    else relative_path_handler(path);
}

void pwd_handler() {
    cout << cur->pwd << endl;
}

void normal_ls_handler(int sta) {
    // if sta == 1, don't deploy hiding dirs and files
    cout << "directories: " << endl;
    cout << "." << "\t" << ".." << "\t";
    for (auto x : cur->sons) {
        if (x->name[0] == '.' && sta == 1) continue;
        cout << x->name << "\t";
    }
    cout << endl;

    cout << "files: " << endl;
    for (auto x : cur->files) {
        if (x->name[0] == '.' && sta == 1) continue;
        cout << x->name << "\t";
    }
    cout << endl;
}

string convert_month_to_string(int mon) {
    char month[13][5] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    return month[mon];
}

void time_handler(struct tm* ttime) {
    string mon = convert_month_to_string(ttime->tm_mon);
    cout << mon;
    printf(" %2d  ", ttime->tm_mday);
    printf("%2d:%02d\t", ttime->tm_hour, ttime->tm_min);
}


void detail_ls_handler() {
    int sz = 0;
    sz += cur->sons.size();
    sz += cur->files.size();
    cout << "total: " << sz << endl;
    cout << "directories: " << endl;
    for (auto x : cur->sons) {
        //authority
        for (int i = 0; i < 3; i++) cout << x->authority[i];
        cout << "\t";
        cout << x->owner << " " << x->group << "\t";
        time_handler(x->currentTime);
        cout << x->name << endl;
    }
    cout << "files: " << endl;
    for (auto x : cur->files) {
        //authority
        for (int i = 0; i < 3; i++) cout << x->authority[i];
        cout << "\t";
        cout << x->owner << " " << x->group << "\t";
        time_handler(x->currentTime);
        cout << x->name << endl;
    }
}

void ls_handler(string ls_cmd) {
    if (ls_cmd != "-a" && ls_cmd != "-l") {
        normal_ls_handler(1);
        return;
    }
    //ls hiding dirs and files;
    if (ls_cmd == "-a") {
        normal_ls_handler(0);
        return;
    }
    if (ls_cmd == "-l") {
        detail_ls_handler();
        return;
    }
}

void mkdir_handler(string dirname) {
    if (dirname.find('/') != string::npos) {
        cout << "illegal name" << endl;
        return;
    }
    dirNode *newdir = new dirNode(dirname, cur);
    cur->sons.insert(newdir);
    cur->mm[dirname] = newdir;
}

void touch_handler(string filename) {
    if (filename.find('/') != string::npos) {
        cout << "illegal name" << endl;
        return;
    }
    cur->files.insert(new fileNode(filename));
}

void rm_dir_handler(string dirname) {
    if (cur->mm.find(dirname) == cur->mm.end()) {
        cout << "No such file or directory" << endl;
        return;
    }
    auto it = cur->mm.find(dirname);
    cur->mm.erase(it);
    for (auto it2 = cur->sons.begin(); it2 != cur->sons.end(); it2++) {
        dirNode* tmp = *it2;
        if (tmp->name == dirname) {
            cur->sons.erase(it2);
            break;
        }
    }
}

void rm_file_handler(string filename) {
    bool flag = false;
    for (auto it = cur->files.begin(); it != cur->files.end(); it++) {
        fileNode* tmpfile = *it;
        if (tmpfile->name == filename) {
            cur->files.erase(it);
            flag = true;
            break;
        }
    }
    if (!flag) {
        cout << "No such file" << endl;
    }
}

void rm_handler(string rmname) {
    if (rmname.substr(0, 3) == "-rf") {
        rm_dir_handler(rmname.substr(4, rmname.size() - 4));
        return;
    }
    if (rmname[0] == '-') {
        cout << "illegal operation" << endl;
        return;
    }
    rm_file_handler(rmname);
}

void handle(string cmd) {
    if (cmd.substr(0, 2) == "cd") {
        string path = cmd.substr(3, cmd.size() - 3);
        cd_handler(path);
    }
    if (cmd.substr(0, 3) == "pwd") {
        pwd_handler();
    }
    if (cmd.substr(0, 2) == "ls") {
        if (cmd == "ls") normal_ls_handler(1);
        else {
            string ls_cmd = cmd.substr(3, cmd.size() - 3);
            ls_handler(ls_cmd);
        }
    }
    if (cmd.substr(0, 5) == "mkdir") {
        string dirname = cmd.substr(6, cmd.size() - 6);
        mkdir_handler(dirname);
    }
    if (cmd.substr(0, 5) == "touch") {
        string filename = cmd.substr(6, cmd.size() - 6);
        touch_handler(filename);
    }
    if (cmd.substr(0, 2) == "rm") {
        string rmname = cmd.substr(3, cmd.size() - 3);
        rm_handler(rmname);
    } 
}

int main() {
    init();
    string cmd;
    while (1) {
        cout << "lz console: ";
        getline(cin, cmd);
        handle(cmd);
        if (cmd == "quit") break;
    }
    return 0;
}