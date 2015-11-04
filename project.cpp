#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

class host
{
	int MAXNUM;
	int MAXINT;
	int *dist;
	int *prev;
	int **routing_table;
public:
	host(int host_num);
	void addLink(int host1, int host2, int dis);
	void Dijkstra(int v0);
	void SearchPath(int v0, int u);
	void PingAll(int this_host);
	void PrintRoutingTable();
	~host();
};
host::host(int host_num)
{
	MAXNUM = host_num;
	MAXINT = 32767;
	prev = new int[MAXNUM];
	dist = new int[MAXNUM];
	for (int i = 0; i < MAXNUM; i++)
		dist[i] = MAXINT;
	routing_table = new int*[MAXNUM];
	for (int i = 0; i < MAXNUM; i++) {
		routing_table[i] = new int[MAXNUM];
		for (int j = 0; j < MAXNUM; j++)
			routing_table[i][j] = MAXINT;
	}
}
void host::addLink(int host1, int host2, int dis)
{
	if (routing_table[host1][host2] > dis) {
		routing_table[host1][host2] = dis;
		routing_table[host2][host1] = dis;
	}
}
void host::Dijkstra(int v0)
{
	bool S[MAXNUM];
	int n = MAXNUM;
	for (int i = 0; i < n; i++) {
		dist[i] = routing_table[v0][i];
		S[i] = false;
		if (dist[i] == MAXINT)
			prev[i] = 0;
		else
			prev[i] = v0;
	}
	dist[v0] = 0;
	S[v0] = 1;

	for (int i = 1; i < n; i++) {
		int temp = MAXINT;
		int u = v0;
		for (int j = 0; j < n; j++) {
			if ((!S[j]) && dist[j] < temp) {
				u = j;
				temp = dist[j];
			}
		}
		S[u] = true;

		for (int j = 0; j < n; j++) {
			if ((!S[j]) && routing_table[u][j] < MAXINT) {
				int newdist = dist[u] + routing_table[u][j];
				if (newdist < dist[j]) {
					dist[j] = newdist;
					prev[j] = u;
				}
			}
		}
	}
}
void host::SearchPath(int v0, int u)
{
	int queue[MAXNUM];
	int tot = 0;
	queue[tot++] = u;
	int temp = prev[u];
	while (temp != v0) {
		queue[tot++] = temp;
		temp = prev[temp];
	}
	queue[tot] = v0;
	for (int i = tot; i >= 0; i--) {
		if (i != 0)
			cout << queue[i] + 1 << " -> ";
		else
			cout << queue[i] + 1 << endl;
	}
}
void host::PingAll(int this_host)
{
	for (int i = 0; i < MAXNUM; i++) {
		if (i == this_host)
			continue;
		SearchPath(this_host, i);
	}
}
void host::PrintRoutingTable()
{
	for (int i = 0; i < MAXNUM; i++) {
		for (int j = 0; j < MAXNUM; j++)
			cout << routing_table[i][j] << "\t\t";
		cout << endl;
	}
}
host::~host()
{
	delete dist;
	delete prev;
	for (int i = 0; i < MAXNUM; i++)
		delete []routing_table[i];
	delete []routing_table;
}

host *Extract(char *buf)
{
	int host_num = buf[0] - '0';
	host *h = new host(host_num);
	buf = buf + 2;
	char *p;

	int host1, host2;
	while (p = strchr(buf, ' ')) {
		*p = 0;
		host1 = atoi(buf);
		buf = p + 1;

		p = strchr(buf, ' ');
		if (p != 0) {
			*p = 0;
			host2 = atoi(buf);
			buf = p + 1;
		}
		else {
			host2 = atoi(buf);
		}

		h->addLink(host1 - 1, host2 - 1, 1);
	}
	return h;
}

void ForkChildren(char *buf)
{
	int host_num = buf[0] - '0';
	host *h = Extract(buf);
	pid_t pid;

	for (int i = 0; i < host_num; i++) {
		pid = fork();
		if (pid < 0) {
			perror("Fork Error: ");
			exit(1);
		}
		else if (pid == 0) {
			h->Dijkstra(i);
			h->PingAll(i);
			exit(0);
		}
		else {
			//Parent
		}
	}

	pid_t wpid;
	int status;
	while ((wpid = wait(&status)) > 0) {
		cout << "Host " << wpid - getpid() << " exited" << endl;
	}
}

void FormatChecking(char *buf)
{
	int counter = -1;
	for (int i = 0; i < strlen(buf); i++) {
		if (buf[i] != ' ' && (buf[i] < '0' || buf[i] > '9'))  {
			cout << "Input Error!" << endl;
			exit(0);
		}
		if (buf[i] > '0' && buf[i] < '9') {
			if (buf[i] > buf[0]) {
				cout << "Host Number Error!" << endl;
			}
			counter++;
		}
	}
	if (counter % 2 != 0) {
		cout << "Input Length Error!" << endl;
		exit(0);
	}
}

int main()
{
	char buf[80];
	strcpy(buf, "4 1 2 2 3 1 4");
	buf[13] = 0;
	FormatChecking(buf);
	ForkChildren(buf);
	return 0;
}
