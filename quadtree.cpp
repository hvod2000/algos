#include <bits/stdc++.h>
using namespace std;

const bool DEBUG = true;
const int QUADTREE_CONTENT_MAX_SIZE = 8;

typedef struct point {
	double x;
	double y;
} Point;

struct object {
	double x;
	double y;
	double radius;
	bool operator ==(const struct object& other) const {
		return this->x == other.x and
			this->y == other.y and
			this->radius == other.radius;
	}
};

namespace std {
	template<>
	struct hash<struct object> { auto operator()( const struct object& obj ) const {
		return std::hash<double>()(obj.x) ^ std::hash<double>()(obj.y);
	}};
}

struct quadtree {
	bool is_leaf;
	union {
		struct quadtree* subtrees[4];   // ^ 1 3
		unordered_set<object> *objects; // y 0 2 x >
	};
};

struct quadtree *new_quadtree() {
	struct quadtree *tree = new struct quadtree();
	tree->is_leaf = true;
	tree->objects = new unordered_set<object>();
	return tree;
}

double dist(double x0, double y0, double x1, double y1) {
	double dx = x1 - x0, dy = y1 - y0;
	return sqrt(dx * dx + dy * dy);
}

double dist(Point O, double side, Point p) {
	double x = max(abs(p.x - O.x) - side / 2, 0.0);
	double y = max(abs(p.y - O.y) - side / 2, 0.0);
	return sqrt(x * x + y * y);
}

tuple<double, double> get_closest_point(Point p1, Point p2, Point target) {
	double dx = p2.x - p1.x, dy = p2.y - p1.y;
	double d2 = dx * dx + dy * dy;
	double t = max(0.0, min(((target.x-p1.x)*dx + (target.y-p1.y)*dy) / d2, 1.0));
	return {dx*t + p1.x, dy*t + p1.y};
}

void split_tree_into_subtrees(struct quadtree &tree, Point p0, double side) {
	unordered_set<object> *objects = tree.objects;
	tree.is_leaf = false;
	for (int quadrant = 0; quadrant < 4; quadrant++) {
		double x0 = ((quadrant / 2) / 2.0 - 0.25) * side + p0.x;
		double y0 = ((quadrant % 2) / 2.0 - 0.25) * side + p0.y;
		struct quadtree *subtree = new_quadtree(); // DANGEROUS MOMENT
		for (auto obj : *objects) 
			if (dist({x0, y0}, side / 2, {obj.x, obj.y}) < obj.radius)
				subtree->objects->insert(obj);
		tree.subtrees[quadrant] = subtree;
	}
	delete objects;
}

void add_object_to_quadtree(struct object obj, struct quadtree &tree, Point p0, double side) {
	if (dist(p0, side, {obj.x, obj.y}) >= obj.radius) return;
	if (tree.is_leaf) {
		tree.objects->insert(obj);
		if (tree.objects->size() > QUADTREE_CONTENT_MAX_SIZE)
			split_tree_into_subtrees(tree, p0, side);
		return;
	}
	for (int quadrant = 0; quadrant < 4; quadrant++) {
		double x0 = ((quadrant / 2) / 2.0 - 0.25) * side + p0.x;
		double y0 = ((quadrant % 2) / 2.0 - 0.25) * side + p0.y;
		add_object_to_quadtree(obj, *tree.subtrees[quadrant], {x0, y0}, side / 2);
	}
}

void remove_object_from_quadtree(struct object obj, struct quadtree &tree, Point p0, double side) {
	if (dist(p0, side, {obj.x, obj.y}) >= obj.radius) return;
	if (tree.is_leaf) {
		tree.objects->erase(obj);
		return;
	}
	for (int quadrant = 0; quadrant < 4; quadrant++) {
		double x0 = ((quadrant / 2) / 2.0 - 0.25) * side + p0.x;
		double y0 = ((quadrant % 2) / 2.0 - 0.25) * side + p0.y;
		remove_object_from_quadtree(obj, *tree.subtrees[quadrant], {x0, y0}, side / 2);
	}
}

bool find_in_tree(struct quadtree &tree, Point target, Point p0, double side, struct object &result) {
	if (dist(p0, side, target) > 0.0) return NULL;
	if (tree.is_leaf) {
		for (struct object obj : *tree.objects) {
			if (dist(obj.x, obj.y, target.x, target.y) < obj.radius) {
				result = obj;
				return true;
			}
		}
		return false;
	}
	for (int quadrant = 0; quadrant < 4; quadrant++) {
		double x0 = ((quadrant / 2) / 2.0 - 0.25) * side + p0.x;
		double y0 = ((quadrant % 2) / 2.0 - 0.25) * side + p0.y;
		if (target.x >= x0 - side / 4 and target.x <= x0 + side / 4 and
			target.y >= y0 - side / 4 and target.y >= y0 + side / 4)
			return find_in_tree(tree, target, {x0, y0}, side / 2, result);
	}
	return false;
}


int main() {
	struct quadtree *tree_ptr = new_quadtree();
	struct quadtree &tree = *tree_ptr;
	Point center = {0, 0};
	double side = 10e9;

	cout << "available commands:" << endl;
	cout << "\tadd x y radius -- add a circle to the quadtree" << endl;
	cout << "\tdel x y radius -- delete a circle from the quadtree" << endl;
	cout << "\tfnd x y -- find object in quadtree by its point" << endl;
	while (not feof(stdin)) {
		cout << "> ";
		string cmd;
		double x, y;
		cin >> cmd >> x >> y;
		if (cmd == "add") {
			double radius; cin >> radius;
			add_object_to_quadtree({x, y, radius}, tree, center, side);
		} else if (cmd == "del") {
			double radius; cin >> radius;
			remove_object_from_quadtree({x, y, radius}, tree, center, side);
		} else if (cmd == "fnd") {
			struct object result;
			if (find_in_tree(tree, {x, y}, center, side, result))
				cout << "found: " << result.x << " " << result.y << " " << result.radius << endl;
			else
				cout << "not found" << endl;
		}
	}
}
