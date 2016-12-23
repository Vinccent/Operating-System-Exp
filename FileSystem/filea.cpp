#include <bits/stdc++.h>

using namespace std;

struct node {
	string name;
	string pwd;
	node *fa;
	//save dirs
	set<node*> sons;
	//save files
	set<string> files;
	map<string, node*> basic;

	
	node(string s) {
		name = s;
		fa = NULL;
		files.clear();
	}

	node(node *pa, string s) {
		name = s;
		pwd = pa->pwd + s + "/";
		fa = pa;
		files.clear();
	}

	void ls() {
		cout << "Directories: " << endl;
		cout << "." << "\t" << ".." << "\t";
		for (auto t : sons) {
			if (t != fa && t != this) {
				cout << t->name << "\t";
			}
		}
		cout << endl;
		cout << "Files: " << endl;
		for (auto t : files) {
			cout << t << "\t";
		}	
		cout << endl;
	}

};

node* cur;
node* root;

void init() {
	root = new node("/");
	root->name = "root";
	root->pwd = "/";
	root->basic["."] = root;
	root->basic[".."] = root;	
}

int main() {
	init();
	cur = root;
	while (1) {
		cout << "lz console =v=: ";
		string cmd;
		getline(cin, cmd);

		if (cmd == "quit") break;

		//call cd
		if (cmd.substr(0, 2) == "cd") {
				string t = cmd.substr(3, cmd.size() - 3);
				if (t == ".") {
					continue;
				}
				if (t == "..") {
					if (cur == root) continue;
					cur = cur->fa;
					continue;
				}
				if (cur->basic.find(t) == cur->basic.end()) {
					cout << "No such folder" << endl;
				} else {
					node *next = cur->basic[t];
					cur = next;
				}
				continue;
		}

		//call pwd
		if (cmd == "pwd") {
			cout << cur->pwd << endl;
			continue;
		}

		//call ls
		if (cmd.substr(0, 2) == "ls") {
			cout << "Directories: " << endl;
			cout << "." << "\t" << ".." << "\t";
			for (auto t : cur->sons) {
				if (t == cur->fa || t == cur) continue;
				cout << t->name << "\t";
			}
			cout << endl;
			cout << "Files: " << endl;
			for (auto t : cur->files) {
				cout << t << "\t";
			}
			cout << endl;
			continue;
		}

		//call mkdir
		if (cmd.substr(0, 5) == "mkdir") {
			string t = cmd.substr(6, cmd.size() - 6);
			node* tmp = new node(cur, t);
			cur->basic[t] = tmp;
			cur->sons.insert(tmp);
			continue;
		}

		//rmdir
		if (cmd.substr(0, 5) == "rmdir") {
			string t = cmd.substr(6, cmd.size() - 6);
			bool flag = false;
			for (auto it = cur->basic.begin(); it != cur->basic.end(); it++) {
				if (it->first == t) flag = true;
			}
			if (!flag) {
				cout << "No such folder" << endl;
				continue;
			}
			//del map from basic;
			for (auto it = cur->basic.begin(); it != cur->basic.end(); it++) {
				if (it->first == t) {
					//auto tmpit = it++;
					//it--;
					cur->basic.erase(it);
					break;
					//it = tmpit;
				}
			}
			//del node from sons;
			for (auto it = cur->sons.begin(); it != cur->sons.end(); it++) {
				node *tnode = *it;
				if (tnode->name == t) {
					cur->sons.erase(it);
					break;
				}
			}
			continue;
		}

		//touch file
		if (cmd.substr(0, 5) == "touch") {
			string t = cmd.substr(6, cmd.size() - 6);
			if (cur->files.find(t) != cur->files.end()) {
				cout << "File already exist" << endl;
				continue;
			}
			cur->files.insert(t);
			continue;
		}

		//rm file
		if (cmd.substr(0, 2) == "rm") {
			string t = cmd.substr(3, cmd.size() - 3);
			if (cur->files.find(t) == cur->files.end()) {
				cout << "No such file" << endl;
				continue;
			}
			for (auto it = cur->files.begin(); it != cur->files.end(); it++) {
				if (*it == t) {
					cur->files.erase(it);
					break;
				}
			}
			continue;
		}
	}
	return 0;
}