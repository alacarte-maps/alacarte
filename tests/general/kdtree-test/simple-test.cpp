#include "includes.hpp"
#include "../../tests.hpp"
#include "general/geodata.hpp"
#include "utils/transform.hpp"
#include "general/nodeKdTree.hpp"
#include "general/node.hpp"

BOOST_AUTO_TEST_SUITE(tree_test)

struct tree_test
{
	shared_ptr<NodeKdTree> tree;
	shared_ptr<std::vector<Node> > nodes;

	tree_test(shared_ptr<std::vector<Node> > nodes)
	: nodes(nodes)
	{
		shared_ptr<std::vector<FixedPoint>> points = boost::make_shared<std::vector<FixedPoint>>();
		for (auto& n : *nodes)
			points->push_back(n.getLocation());
		tree = boost::make_shared<NodeKdTree>(points);
		tree->buildTree();
	}

	void search(const FixedRect& r, const std::vector<int>& ids)
	{

		shared_ptr<std::vector<NodeId> > result = boost::make_shared< std::vector<NodeId> >();
		tree->search(result, r);
		BOOST_TEST_MESSAGE("Returned nodes: " << result->size());
		BOOST_CHECK_EQUAL(result->size(), ids.size());

		std::sort(result->begin(), result->end(),
				  [](NodeId first, NodeId second)
					{
					  return first.getRaw() < second.getRaw();
					}
				 );
		bool sameResult = true;
		for (int i = 0; i < result->size(); i++) {
			if (result->at(i).getRaw() != ids.at(i)) {
				sameResult = false;
				break;
			}
		}
		BOOST_CHECK(sameResult);
	}
};

struct tile_test {
	tree_test* imp;
	static const int nodeCount = 20;
	tile_test () {
		shared_ptr<std::vector<Node> > nodes = boost::make_shared<std::vector<Node> >();
		//In 16/34297/22502
		for(int i = 0; i < nodeCount; ++i)
		{
			double lon = 8.39924 + (rand()%1) / 10.0; //between 8.39924 and 8.40435 is valid
			double lat = 49.01619 - (rand()%1) / 1000.0; //between 49.01619 and 49.01305 is valid
			nodes->push_back(Node(FloatPoint(lon, lat), DataMap<CachedString, CachedString>()));
		}
		imp = new tree_test(nodes);
	}

	void search()
	{
		int x = 34297;
		int y = 22502;
		int zoom = 16;
		coord_t x0, y0, x1, y1;
		tileToMercator(x,   y,   zoom, x0, y0);
		tileToMercator(x+1, y+1, zoom, x1, y1);
		std::vector<int> expectedIds;
		for(int i = 0; i < nodeCount; ++i)
		{
			expectedIds.push_back(i);
		}
		imp->search(FixedRect(FixedPoint(x0, y0), FixedPoint(x1, y1)), expectedIds);
	}
};

ALAC_START_FIXTURE_TEST(tile_test)
	ALAC_FIXTURE_TEST(search);
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
