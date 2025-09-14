#include <bits/stdc++.h>
using namespace std;

class Graph {
  private:
   vector<vector<pair<int, int>>> adj;  // adjacency list
  public:
   Graph(int size = 500) { adj.resize(size); }
   void addEdge(int u, int v, int w) {
      adj[u].push_back({v, w});
      adj[v].push_back({u, w});
   }
   const vector<pair<int, int>>& getNeighbors(int node) const {
      return adj[node];
   }
};

class RobotPathfinder {
  private:
   int N, M;
   int startHour;
   string startNode, targetNode;
   Graph graph;
   map<string, int> nameToId;
   vector<string> idToName;
   vector<int> nodeRoles;  // role: R, C, M, E
   bool initialized = false;
   int nextId = 0;

   int getNodeId(const string& name) {
      if (nameToId.count(name)) return nameToId[name];
      int id = nextId++;
      nameToId[name] = id;
      idToName.push_back(name);
      return id;
   }

   void setRole(const string& node, char role) {
      int id = getNodeId(node);
      nodeRoles[id] = (int)role;
   }

   void MunculkanError(const string& msg) {
      cerr << "[ERR0R] " << msg << endl;
      exit(1);
   }

  public:
   RobotPathfinder() : graph(500), nodeRoles(500, -2) {}

   void init() {
      cin >> N >> M;
      if (N <= 0 || M <= 0 || N > 200 || M > 2000) {
         MunculkanError("Input melebihi batasan atau tidak sesuai.");
      }

      // input edges
      for (int i = 0; i < M; i++) {
         string u, v;
         int w, o;
         cin >> u >> v >> w >> o;
         int uid = getNodeId(u);
         int vid = getNodeId(v);
         graph.addEdge(uid, vid, w + o);
      }

      // input start & target
      cin >> startNode >> targetNode;
      cin.ignore();

      // input roles
      for (char role : string("RCME")) {
         string line;
         getline(cin, line);
         stringstream ss(line);
         string token;
         while (ss >> token) {
            if (token == "-") break;
            setRole(token, role);
         }
      }

      cin >> startHour;
      initialized = true;
   }

   void findPath(int speed) {
      if (!initialized) MunculkanError("Belum dipanggil init().");

      int startId = getNodeId(startNode);
      int targetId = getNodeId(targetNode);

      // best[node][energy] = energi minimum
      vector<vector<int>> best(nextId, vector<int>(1001, INT_MAX));

      using State = tuple<int, int, int, int, vector<int>, vector<int>>;
      priority_queue<State, vector<State>, greater<>> pq;

      pq.push({0, startId, 0, 1000, {startId}, {0}});
      best[startId][1000] = 0;

      while (!pq.empty()) {
         auto [energyUsed, node, timeNow, energyLeft, path, times] = pq.top();
         pq.pop();

         // kalau sampai tujuan
         if (node == targetId) {
            cout << "Total energi minimum: " << energyUsed << "\n";
            cout << "Jalur: ";
            for (int i = 0; i < path.size(); i++) {
               if (i) cout << " -> ";
               cout << idToName[path[i]];
            }
            cout << "\nWaktu tiba:\n";
            for (int i = 0; i < path.size(); i++) {
               cout << idToName[path[i]] << " (Jam "<< startHour + times[i] / 60 << " Menit " << times[i] % 60<< ")\n";
            }
            return;
         }

         // kalau di Restpoint (R), tunggu kalau masuk jam ganjil
         if (nodeRoles[node] == (int)'R') {
            int hour = startHour + (timeNow / 60);
            if (hour % 2 == 1) {
               int delay = 60 - (timeNow % 60);
               timeNow += delay;
            }
         }

         // coba semua edge
         for (auto& e : graph.getNeighbors(node)) {
            int nxt = e.first;
            int w = e.second;

            int newTime = timeNow + (int)ceil((double)w / speed);
            int hour = startHour + (timeNow / 60);

            // faktor energi: genap = hemat, ganjil = boros
            double faktor = (hour % 2 == 1 ? 1.3 : 0.8);
            int cost = (int)ceil(w * faktor);

            int newEnergy = energyLeft;
            if (nodeRoles[node] == (int)'C') {
               newEnergy = 1000;  // recharge
            }
            if (newEnergy < cost) continue;

            newEnergy -= cost;
            int newEnergyUsed = energyUsed + cost;

            if (newEnergyUsed < best[nxt][newEnergy]) {
               best[nxt][newEnergy] = newEnergyUsed;
               auto newPath = path;
               auto newTimes = times;
               newPath.push_back(nxt);
               newTimes.push_back(newTime);
               pq.push({newEnergyUsed, nxt, newTime, newEnergy, newPath, newTimes});
            }
         }
      }

      cout << "Robot gagal dalam mencapai tujuan :(\n";
   }
};

int main() {
   ios::sync_with_stdio(false);
   cin.tie(nullptr);

   RobotPathfinder robot;
   robot.init();
   robot.findPath(100);  // speed bisa diganti
}
