#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <queue>
#include <fstream>
using namespace std;
struct node {
	int id;//unique id
	int lvl;//floor number statrting from -1 when creating a building
	string name;//building name or full name
	map<int, node*> neighbors;
};
class graph {
private:
	map<int, node*> all_nodes;
	map<string, node*> building_names;
	//stores data like this (SB){1,2,3,4,5} 
	//making the building name a key to a set of its floors
	map<string, set<int>> building_floors;
	int global_id= 0;
public:
	
	// adding a room requires the building name, the floor (number ) and the id of the room
	//input should be something like ("SB",1,33) so SB-1-33 is the room created
	// this should also be added to the map all_nodes with id starting at 500
	//creating room in an un-avilabile building is an error
	//creating room in an un-avilabile floor is an error
	//1
	void add_room(string b_name, int f_num, int r_id) {
		if (!building_names.count(b_name)) {
			cout << b_name << " building does not exist\n"; return;
		}
		if (!building_floors[b_name].count(f_num)) {
			cout << b_name << " floor does not exist\n";
			return;
		}
		node* x = new node;
		x->lvl = 2;
		x->name = b_name + "-" + to_string(f_num) + "-" + to_string(r_id);
		x->id = ++global_id;
		all_nodes[x->id] = x;
		string floor_target = b_name + "-" + to_string(f_num);
		for (auto const& item : all_nodes) {//this searches every item in a map (both key and value)
			int id = item.first;      // This is the key
			node* n = item.second;    // This is the value

			if (n->name == floor_target) {
				add_edge(x->id, id);
				//delete n;
				break;
			}
		}
			cout << "Room " << x->name << " created.\n";
	}
	//adding a floor requires the building name
	//input should be ("SB") output is SB-1 created meaning SB now has SB-0 and SB-1
	// this should also be added to the map all_nodes with id starting at 200
	//2
	void add_floor(string b_name) {
		if (!building_names.count(b_name)) {
			cout << b_name << " building does not exist\n"; return;
		}
		int floor_num = building_floors[b_name].size();

		node* x = new node;
		x->id = ++global_id;                    
		x->lvl = 1;
		x->name = b_name + "-" + to_string(floor_num);
		all_nodes[x->id]=x;
		building_floors[b_name].insert(floor_num);
		add_edge(x->id, building_names[b_name]->id);
		cout << "Floor " << x->name << " created.\n";
	}

	//creating a building only requires the name of said building
	//input is ("SS") output is building SS has been created 
	// this should also be added to the map all_nodes with id starting at 000
	//3
	void add_building(string b_name) {
		if (building_names.count(b_name)) {
			cout << b_name << " building already exists\n"; return;
		}
		node* x=new node;
		
		x->id =++global_id;
		x->name = b_name;
		x->lvl = 0;
		all_nodes[x->id] = x;
		building_names[b_name] = x;
		cout << "Building " << b_name << " created.\n";
	}

	//takes room whole name SB-1-04 
	//gives the path SB -> first floor -> room 03
	//obvoice and not compilcated code
	// remember verifying the info
	//4
	void get_room() {
		string b;
		int f, r;
		cout << "Enter the building ur looking for(name): ";
		cin >> b;
		if (!building_names.count(b)) {
			cout << "Building dont exist\n"; 
			return;
		}
		cout << "Enter the floor ur looking for (as a number): ";
		cin >> f;
		if (!building_floors[b].count(f)) {
			cout << "Floor dont exist\n"; 
			return;
		}
		cout << "Enter the room ur looking for (id): ";
		cin >> r;
		node* temp = new node;
		temp->name = b + "-" + to_string(f) + "-" + to_string(r);
		for (auto const& item : all_nodes) {
			if (item.second->name == temp->name) {
				cout << "From building: " << b << " go to floor: " << f << " then to room: " << r << endl;return;
			}
		}
		delete temp;
	}

	//give back how many floors a building has
	//5
	void get_floor() {
		string b;
		cout << "Enter the building ur looking for(name): ";
		cin >> b;
		if (!building_names.count(b)) {
			cout << "Building dont exist\n"; 
			return;
		}
		bool found = false;
		for (auto const& item : building_floors) {
			if (item.first == b) {
				for (auto it = item.second.begin(); it != item.second.end(); ++it)
					cout << *it << " ";
				found = true;
			}
		}
		if (found == false)
			cout << "room dont exist\n";
	}

	//gets the number of buildings
	// prints the keys in building_names which are the names 
	//6
	void get_buildings(){
		cout << "Campus Buildings: \n";
		for (auto const&item : building_names) cout << "- " << item.first << endl;
	}

	//gets path between 2 rooms (even if in dif buildings or floors)
	//7
	// BFS Pathfinding: Finds the shortest steps between two room names
	void get_path(string start_name, string end_name) {
		node* startNode = nullptr, * endNode = nullptr;//create nodes to point at both end/begining nodes
		for (auto const&item : all_nodes) {//search all_nodes for both begin/end nodes
			if (item.second->name == start_name) startNode = item.second;
			if (item.second->name == end_name) endNode = item.second;
		}

		if (!startNode || !endNode) {
			cout << "One or both rooms not found!\n"; return;
		}

		queue<node*> q;//queue to save nodes between wanted nodes
		map<int, int> parent_map; // To reconstruct the path
		set<int> visited;//set that saves the nodes we been to by ID since its unique 

		//push and insert node and its id to queue and set
		q.push(startNode);
		visited.insert(startNode->id);

		bool found = false;
		while (!q.empty()) {//until queue is empty we make temp node
			node* curr = q.front();
			q.pop();

			//temp node checks if the next node in the path is the wanted node
			if (curr->id == endNode->id) {
				found = true; break;
			}
			//we search temp nodes neighbors to find the last (wanted) id that we stored in visited 
			//search is through the neighbors maps id's 
			for (auto const& item : curr->neighbors) {//id first, ptr second
				if (visited.find(item.first) == visited.end()) {
					visited.insert(item.first);//insert unseen before node id to visited
					//tracks reaching the id to the temp node
					//allows the format:ROOM → FLOOR → BUILDING → FLOOR → ROOM
					parent_map[item.first] = curr->id;
					q.push(item.second);
				}
			}
		}

		if (found) {
			vector<string> path;//saves the nodes in the path
			int curr_id = endNode->id;
			while (curr_id != startNode->id) {
				//pushing nodes names into path 
				path.push_back(all_nodes[curr_id]->name);
				//curr_id equals next id so we could push all the path into vector (path)
				curr_id = parent_map[curr_id];
			}
			path.push_back(startNode->name);//last thing we push is the first node and first is the end node
			reverse(path.begin(), path.end());//revesre func to reverse vector

			cout << "Path found: ";
			for (int i = 0; i < path.size(); i++) {
				cout << path[i];
				if (i == path.size() - 1) {
					cout<< "";
				}
				else {
					cout<< " -> ";
				}
			}
			cout << endl;
		}
		else {
			cout << "No path exists\n";
		}
	}

	//gets left and or right rooms to each room (same floor and building)
	// takes building name , floor number , and room id
	//8
	void get_neighbors() {
		string b;
		int f, r;
		cout << "Enter the building ur looking for(name): ";
		cin >> b;
		if (!building_names.count(b)) {
			cout << "Building dont exist\n";
			return;
		}
		cout << "Enter the floor ur looking for (as a number): ";
		cin >> f;
		if (!building_floors[b].count(f)) {
			cout << "Floor dont exist\n"; 
			return;
		}
		cout << "Enter the broom ur looking for (id): ";
		cin >> r;
		node* temp = new node;
		temp->name = b + "-" + to_string(f) + "-" + to_string(r);
		bool found = false;
		int temp_id;
		for (auto const& item : all_nodes) {
			if (item.second->name == temp->name) {
				temp_id = item.second->id;
				found = true;
			}
		}
		if (found == false) {
			cout << "room dont exist\n";
				return;
		}
		else {
			cout << "neighbors of node: " << temp->name<<" are: ";
			for (auto const& item : all_nodes[temp_id]->neighbors) {
				cout << "- " << item.second->name;
			}
		}
		delete temp;
	}
	//gets number of rooms each building has, considering every floor in said buildng
	//takes building name
	//9
	void room_count() {
		string b;
		cout << "Enter the building name: ";
		cin >> b;
		if (!building_names.count(b)) {
			cout << "Building does not exist\n";
			return;
		}

		int total_rooms = 0;
		// We iterate through all nodes and check their names
		// A room name looks like SB-1-22, so it starts with "SB-"
		string prefix = b + "-";

		for (auto const& item : all_nodes) {
			node* n = item.second;
			// Check if the name starts with "BuildingName-" AND has two dashes (meaning it's a room)
			if (n->name.find(prefix) == 0) {
				// Count dashes to confirm it's a room (Building-Floor-Room)
				int dashes = 0;
				for (char c : n->name) if (c == '-') dashes++;

				if (dashes == 2) {
					total_rooms++;
				}
			}
		}
		cout << "Building " << b << " has a total of " << total_rooms << " rooms.\n";
	}

	//connects rooms->floors, and floors->buildings and vice versa via neighbors map	
	void add_edge(int a, int b) {
		if (all_nodes.count(a) && all_nodes.count(b)) {
			all_nodes[a]->neighbors[b] = all_nodes[b];
			all_nodes[b]->neighbors[a] = all_nodes[a];
		}
	}
	//Connect all buildings together
	void connect_all_buildings() {
		if (building_names.size() < 2) return;

		auto it = building_names.begin();
		while (next(it) != building_names.end()) {
			add_edge(it->second->id, next(it)->second->id);
			it++;
		}
	}

	//Connect rooms on the same floor like a hallway (Room 1 <-> Room 2 <-> Room 3)
	void connect_rooms(string b_name, int f_num) {
		string floor_prefix = b_name + "-" + to_string(f_num) + "-";
		vector<int> room_ids;

		// Collect all room ids for this specific floor
		for (auto const& item : all_nodes) {
			if (item.second->name.find(floor_prefix) == 0) {
				room_ids.push_back(item.first);
			}
		}

		// Connect them in a sequence
		for (int i = 0; i + 1 < room_ids.size(); i++) {
			add_edge(room_ids[i], room_ids[i + 1]);
		}
	}

	//this prints the whole graph
	void print_graph() {
		cout << "\n--- CAMPUS HIERARCHY MAP ---\n";
		for (auto const& b_item : building_names) {
			node* b = b_item.second;
			cout << "[Building] " << b->name << endl;

			for (auto const& f_conn : b->neighbors) {
				node* f = f_conn.second;
				// A floor is level 1 and its name should start with the building name
				if (f->lvl == 1 && f->name.find(b->name) == 0) {
					cout << "  L__ [Floor] " << f->name << endl;

					for (auto const& r_conn : f->neighbors) {
						node* r = r_conn.second;
						// Check if it's a room (Level 2)
						if (r->lvl == 2 && r->name.find(f->name) == 0) {
							cout << "      |-- [Room] " << r->name << endl;
						}
					}
				}
			}
		}
		cout << "----------------------------\n";
	}
	// file functions: 
	void read_room_file() {
		ifstream in("rooms.txt");
		if (!in) {
			cout << "Failed to open file\n";
			return;
		}
		string b_name;
		int f_num, r_id;
		while (in >> b_name >> f_num >> r_id) {
			add_room(b_name, f_num, r_id);
		}
		in.close();
	}

	void read_floor_file() {
		ifstream in("floors.txt");
		if (!in) {
			cout << "Failed to open file\n";
			return;
		}
		string b_name;
		int f_num;
		while (in >> b_name >> f_num) {
			add_floor(b_name);
		}
		in.close();
	}

	void read_building_file() {
		ifstream in("buildings.txt");
		if (!in) {
			cout << "Failed to open file\n";
			return;
		}
		string b_name;
		while (in >> b_name) {
			add_building(b_name);
		}
		in.close();
	}

	void write_room_file() {
		ofstream out("rooms.txt");
		for (auto const& item : all_nodes) {
			node* n = item.second;
			if (n->lvl == 2) { // Only save Room level nodes
				// Parse the name back to components or just store the name
				// To reuse add_room, we store: Building Floor RoomID
				int first_dash = n->name.find('-');
				int last_dash = n->name.find_last_of('-');
				string b = n->name.substr(0, first_dash);
				string f = n->name.substr(first_dash + 1, last_dash - first_dash - 1);
				string r = n->name.substr(last_dash + 1);
				out << b << " " << f << " " << r << endl;
			}
		}
		out.close();
	}

	void write_floor_file() {
		string b_name;
		ofstream out("floors.txt");
		for (auto const& item : building_floors) {
			b_name = item.first;
			for (int f_num : item.second) {
				out << b_name << " " << f_num << endl;
			}
		}
		out.close();
	}

	void write_building_file() {
		ofstream out("buildings.txt");
		for (auto const& item : building_names) {
			out << item.first << endl;
		}
		out.close();
	}
	void save_all() {
		write_building_file();
		write_floor_file();
		write_room_file();
		cout << "All data saved to structure files.\n";
	}

	// Helper to load everything at once
	void load_all() {
		read_building_file();
		read_floor_file();
		read_room_file();
		connect_all_buildings(); // Ensure buildings connect after loading
		// Connect rooms for each floor
		for (auto const& item : building_floors) {
			for (int f : item.second) connect_rooms(item.first, f);
		}
	}
};//end of graph 


void clearTerminal() {
#ifdef _WIN32
	system("cls");  // Windows
#endif
}
int main() {
	graph AUM;
	AUM.load_all();
	clearTerminal();
	int choice, ID;
	cout << "--- AUM INTERNAL GPS ---\nID: ";
	cin >> ID;
	while (true) {
		if (ID >= 101000) { // STUDENT
			cout << "Considering ur a Student, ur allowed these actions:\n";
			cout << "1- locate a room\n";//building -> floor -> room
			cout << "2- get path between 2 rooms\n";//gives the whole path from one room to another 
			//(works on rooms from different building or floors)
			cout << "3- how many floors a building has\n";
			cout << "4- how many buildings there are\n";//prints all buildings names
			cout << "5- how many rooms a building has in each floor\n";
			cout << "6- get neighboring rooms to a certin room\n";
			cout << "7- traverse and print whole campus\n";
			cout << "8- exit\n";
			AUM.print_graph();
			cout << "\nPlease enter what action u would like to take by number: "; cin >> choice;
			clearTerminal(); // Clear the menu to show the result clearly
			switch (choice) {
			case 1: AUM.print_graph(); AUM.get_room(); break;
			case 2: {
				AUM.print_graph();
				string s, e; 
				cout << "Start Room: "; cin >> s;
				cout << "End Room: "; cin >> e;
				AUM.get_path(s, e); break;
			}
			case 3: AUM.print_graph(); AUM.get_floor(); break;
			case 4:AUM.print_graph(); AUM.get_buildings(); break;
			case 5:AUM.print_graph(); AUM.room_count(); break;
			case 6:AUM.print_graph(); AUM.get_neighbors(); break;
			case 7:AUM.print_graph(); AUM.print_graph(); break;
			case 8: return 0;
			default: cout << "Invalid choice.\n";
			}

			// Pause so the user can actually read the output before the menu returns
			cout << "\nPress Enter to return to menu...";
			cin.ignore();
			cin.get();
			clearTerminal();

		}
		else { // ADMIN
			cout << "Considering ur an Admin, ur allowed these actions:\n";
			cout << "1- add a room\n";//building -> floor -> room
			cout << "2- add a floor\n";// bilding-> floor 
			cout << "3- add building\n";
			cout << "4- traverse and print whole campus\n";
			cout << "5- save changes\n";
			cout << "6- exit\n";
			AUM.print_graph();
			cout << "\nPlease enter what action u would like to take by number: "; cin >> choice;
			clearTerminal();
			switch (choice) {
			case 1: {
				AUM.print_graph();
				string b; 
				int f, r;
				cout << "Enter Building Name, Floor, and Room ID: ";
				cin >> b >> f >> r; 
				AUM.add_room(b, f, r); break;
			}
			case 2: {
				AUM.print_graph(); string b;
				cout << "Building Name: "; 
				cin >> b; 
				AUM.add_floor(b); break; }
			case 3: {
				AUM.print_graph(); string b;
				cout << "Building Name: ";
				cin >> b; 
				AUM.add_building(b); 
				break; }
			case 4: AUM.print_graph(); break;
			case 5:AUM.save_all(); break;
			case 6: return 0;
			}

			cout << "\nPress Enter to return to menu...";
			cin.ignore();
			cin.get();
			clearTerminal();
		}
	}
}