
#include "../../tests.hpp"
#include "../../shared/compare.hpp"

#include <boost/filesystem/operations.hpp>

#include "general/geodata.hpp"
#include "general/nodeKdTree.hpp"
#include "general/node.hpp"

BOOST_AUTO_TEST_SUITE(tree_test)

class NodeKdTreeInspector : public NodeKdTree {
public:
	NodeKdTreeInspector(const shared_ptr<NodeKdTree>& nodeTree)
		: NodeKdTree(*nodeTree), maxDepth(0)
	{
	}

	void print(const char* path)
	{
		std::ofstream out;
		out.open(path);
		out << "max depth: " << maxDepth << "\n";
		out << "size of leafs: ( ";
		for (auto& size : leafSize)
			out << size << " ";
		out << ")\n";

		out << "nodes: ( ";
		for (auto& p : nodes)
			out << "(" << p.x << ","  << p.y << "), ";
		out << ")\n";

		out << "split lines: ( ";
		for (auto& l : splitLines) {
			out << "((" << l.start.x << ","  << l.start.y << "),";
			out <<  "(" << l.end.x   << ","  << l.end.y   << ")), ";
		}
		out << ")\n";

		out.close();
	}

	void inspect()
	{
		FixedRect globalRect = FixedRect (
			 std::numeric_limits<coord_t>::min(),
			 std::numeric_limits<coord_t>::min(),
			 std::numeric_limits<coord_t>::max(),
			 std::numeric_limits<coord_t>::max()
						  );

		std::vector<NodeKdTree::SearchStackEntry> stack;
		stack.push_back(SearchStackEntry(root, globalRect, 0));

		do {
			SearchStackEntry se = stack.back();
			stack.pop_back();

			FixedRect leftRect = se.rect;
			FixedRect rightRect = se.rect;
			shared_ptr<kdNode> node = se.node;
			int depth = se.depth;

			maxDepth = std::max(maxDepth, depth);

			// no child nodes
			if (!node->left && !node->right) {
				leafSize.push_back(node->ids.size());
				for (auto id : node->ids)
					nodes.push_back(points->at(id.getRaw()));
				continue;
			}

			// at least one child node
			if ( ( depth % 2 ) == 0 ) {
				coord_t x = node->key;
				leftRect.maxX  = x;
				rightRect.minX = x;
				splitLines.push_back(Line(FixedPoint(x, leftRect.minY), FixedPoint(x, leftRect.maxY)));
			} else {
				coord_t y = node->key;
				leftRect.maxY  = y;
				rightRect.minY = y;
				splitLines.push_back(Line(FixedPoint(leftRect.minX, y), FixedPoint(leftRect.maxX, y)));
			}

			if (node->left)
				stack.push_back(SearchStackEntry(node->left, leftRect, depth+1));
			if (node->right)
				stack.push_back(SearchStackEntry(node->right, rightRect, depth+1));
		} while (!stack.empty());
	}

private:
	struct Line {
		Line(FixedPoint p1, FixedPoint p2) : start(p1), end(p2) {}
		FixedPoint start;
		FixedPoint end;
	};
	std::vector<int> leafSize;
	std::vector<Line> splitLines;
	std::vector<FixedPoint> nodes;
	int maxDepth;
};

/*
class GeodataInspector : public Geodata {
public:
	GeodataInspector(const shared_ptr<Geodata>& data)
		: Geodata(*data)
	{
	}

	const shared_ptr<NodeKdTree>& getNodeTree()
	{
		return nodesTree;
	}
};
*/

struct distribution_test
{
	shared_ptr<Geodata> data;
	//shared_ptr<GeodataInspector> geoInspector;
	shared_ptr<NodeKdTreeInspector> treeInspector;

	distribution_test()
	{
	/*
		path testData = getInputDirectory() / "karlsruhe_big.carte";
		BOOST_CHECK(boost::filesystem::exists(testData));

		data = boost::make_shared<Geodata>();
		data->load(testData.string());

		geoInspector = boost::make_shared<GeodataInspector>(data);
		treeInspector = boost::make_shared<NodeKdTreeInspector>(geoInspector->getNodeTree());
	*/
	}

	void checkLog(const char* name)
	{
	/*
		string file = string(name) + ".log";
		string p = (getRenderedDirectory() / file).native();
		treeInspector->inspect();
		treeInspector->print(p.c_str());

		compareFile(file.c_str());
	*/
	}
};

ALAC_START_FIXTURE_TEST(distribution_test)
	ALAC_FIXTURE_TEST(checkLog, "nodeTree");
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
