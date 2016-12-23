#include <bits/stdc++.h>

using namespace std;

const int maxn = 1000000 + 5;
const int queue_size = 3;

int total_instruction;
int a[maxn];

double cal(double pageFault, int n) {
	double res = pageFault * 100  / (double)n;
	return 100.00 - res;
}

double FIFO(int n) {
	queue<int> q;
	int vis[100], pageFault = 0;
	memset(vis, 0, sizeof(vis));
	for (int i = 0; i < n; i++) {
		if (q.size() < 3) {
			if (vis[a[i]]) continue;
			pageFault++;
			vis[a[i]] = 1;
			q.push(a[i]);
		} else {
			if (vis[a[i]]) continue;
			vis[a[i]] = 1;
			int t = q.front(); q.pop();
			q.push(a[i]);
			vis[t] = 0;
			pageFault++;
		}
	}
	return cal(pageFault, n);
}

double LRU(int n) {
	queue<int> q;
	int vis[100], pageFault = 0;
	memset(vis, 0, sizeof(vis));
	for (int i = 0; i < n; i++) {
		if (q.size() < 3) {
			if (vis[a[i]]) {
				int sz = q.size();
				for (int j = 0; j < sz; j++) {
					int t = q.front(); q.pop();
					if (t != a[i]) q.push(t);
				}
				q.push(a[i]);
			} else {
				pageFault++;
				q.push(a[i]);
				vis[a[i]] = 1;
			}
		} else {
			if (vis[a[i]]) {
				for (int j = 0; j < 3; j++) {
					int t = q.front(); q.pop();
					if (t != a[i]) q.push(t);
				}		
				q.push(a[i]);
			} else {
				int t = q.front(); q.pop();
				vis[t] = 0;
				vis[a[i]] = 1;
				q.push(a[i]);
				pageFault++;
			}
		}
	}
	return cal(pageFault, n);
}


const int CLEAR_PERIOD = 50;
const int maxm = 100;
double NRU(int n) {
	int vis[maxm], pageFault = 0;
	int use[maxm];
	queue<int> q;
	memset(vis, 0, sizeof(vis));
	memset(use, 0, sizeof(use));
	int period = 0;
	for (int i = 0; i < n; i++, ++period) {
		if (period == 50) {
			memset(use, 0, sizeof(use));
			period = 0;
		}
		if (q.size() < 3) {
			use[a[i]] = 1;
			if (!vis[a[i]]) {
				q.push(a[i]);
				vis[a[i]] = 1;
				pageFault++;
			} 
		} else {
			if (vis[a[i]]) {
				use[a[i]] = 1;
				continue;
			}
			pageFault++;
			int replace = 0;
			for (int j = 1; j <= 5; j++) {
				if (vis[j] && !use[j]) {
					replace = j;
				}
			}
			if (replace == 0) {
				int t = q.front(); q.pop();
				vis[t] = 0;
				use[t] = 0;
				vis[a[i]] = 1;
				use[a[i]] = 1;
				q.push(a[i]);
				continue;
			}
			for (int j = 0; j < queue_size; j++) {
				int t = q.front(); q.pop();
				if (t != replace) q.push(t);
			}
			q.push(a[i]);
			vis[a[i]] = 1;
			vis[replace] = 0;
			use[a[i]] = 1;
			use[replace] = 0;
			pageFault++;
		}
	}
	return cal(pageFault, n);
}

vector<int> dist[6];
void pre_handle(int n) {
	for (int j = 1; j <= 5; j++) {
		for (int i = 0; i < n; i++) {
			if (a[i] == j) {
				dist[j].push_back(i);
			}
		}
	}
}

double OPT(int n) {
	int vis[maxm], pageFault = 0;
	memset(vis, 0, sizeof(vis));
	pre_handle(n);
    queue<int> q;
	for (int i = 0; i < n; i++) {
		if (q.size() < 3) {
			if (!vis[a[i]]) {
				vis[a[i]] = 1;
				pageFault++;
				q.push(a[i]);
			}
		} else {
			if (vis[a[i]]) continue;
			pageFault++;
			int tmp[10];
			for (int j = 0; j < 3; j++) {
				int t = q.front(); q.pop();
				q.push(t);
                vector<int> tmp = dist[t];
				auto it = upper_bound(dist[t].begin(), dist[t].end(), t);
				tmp[t] = *it - i;
			}	
			int MAX = 0, replace = 0;
			for (int j = 1; j <= 5; j++) {
				if (vis[j]) {
					if (tmp[j] > MAX) {
						MAX = tmp[j];
						replace = j;	
					}
				}
			}
			for (int j = 0; j < 3; j++) {
				int t = q.front(); q.pop();
                if (t != replace) q.push(t);
			}
            q.push(a[i]);
            vis[a[i]] = 1;
            vis[replace] = 0;
		}
	}
    return cal(pageFault, n);
}

void produce_tot_instruction() {
	total_instruction = rand() % maxn;
	if (total_instruction == 0) produce_tot_instruction();
	else return;
}

void produce_a(int n) {
	for (int i = 0; i < n; i++) a[i] = rand() % 5 + 1;
}

void work() {
	double fifo = 0, lru = 0, nru = 0, opt = 0;
	for (int i = 0; i < 100; i++) { 
		//init total and a[];
		produce_tot_instruction();
		produce_a(total_instruction);

		//FIFO

		double res_fifo = FIFO(total_instruction);
		fifo += res_fifo;

		//LRU
		double res_lru = LRU(total_instruction);
		lru += res_lru;

		//NRU
		double res_nru = NRU(total_instruction);
		nru += res_nru;

		//OPT;
		double res_opt = OPT(total_instruction);
		opt += res_opt;
	}
	fifo /= 100.00;
	lru /= 100.00;
	nru /= 100.00;
	opt /= 100.00;
	printf("%.2lf\n", fifo);
	printf("%.2lf\n", lru);
	printf("%.2lf\n", nru);
	printf("%.2lf\n", opt);
}

int main() {
	srand(time(NULL));
	work();
	return 0;
}
