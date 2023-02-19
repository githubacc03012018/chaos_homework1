#include <iostream>
#include <document.h>
#include <fstream>
#include "iostream"
#include <istreamwrapper.h>
#include <vector>
#include <unordered_set>
 
struct Node {
	int value;
	int childrenCount;
	std::vector<Node*> children;

	Node(int childrenCount, int val = 0) : childrenCount(childrenCount), value(val) {}
};

Node* CreateTreeFromJson(const rapidjson::Value& jsonValue) {

	Node* node = new Node(jsonValue["childrenCount"].GetInt());
	for (const rapidjson::Value& child : jsonValue["children"].GetArray()) {
		node->children.push_back(CreateTreeFromJson(child));
	}
	
	return node;
}

inline rapidjson::Document LoadDocument(const std::string& fileName) {
	std::ifstream ifs(fileName);
	assert(ifs.is_open());

	rapidjson::IStreamWrapper isw(ifs);

	rapidjson::Document doc;
	doc.ParseStream(isw);

	if (doc.HasParseError()) {
		std::cout << "Error : " << doc.GetParseError() << '\n';
		std::cout << "Offset : " << doc.GetErrorOffset() << '\n';
	}
	assert(doc.IsObject());
	return doc;
}

bool CanBeGracious(Node* node, int n, std::unordered_set<int>& diffs) {
	int expectedValue = 2 * n - 1;
	node->value = expectedValue;

	for (auto& child : node->children) {
		n++;
		if (!CanBeGracious(child, n, diffs)) {
			return false;
		}

		int difference = std::abs(node->value - child->value);
		if (diffs.find(difference) != diffs.end()) {
			return false;
		}

		diffs.insert(difference);
	}

	return true;
}

int CountNodes(Node* node) {
	int count = 1;
	for (const auto child : node->children) {
		count += CountNodes(child);
	}

	return count;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "How to use: " << argv[0] << " <jsonFile_path>" << "\n";
		return 1;
	}

	const char* jsonFilePath = argv[1];
	
	auto doc = LoadDocument(jsonFilePath);
	Node* root = CreateTreeFromJson(doc);

	std::unordered_set<int> diffs;
	if (CanBeGracious(root, 1, diffs) && CountNodes(root) % 2 != 0) {
		std::cout << "Tree can be gracious!" << std::endl;
	}
	else {
		std::cout << "Tree can not be gracious!" << std::endl;
	}

	return 0;
}