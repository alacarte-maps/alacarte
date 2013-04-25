
#include "../../tests.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "general/geodata.hpp"
#include "utils/transform.hpp"

#include "../../shared/compare.hpp"

#include "geodataMock.hpp"

#include <boost/filesystem/operations.hpp>

BOOST_AUTO_TEST_SUITE(geodata_test)

template<typename IdType>
struct CompareObjects
{
	bool operator ()(IdType first, IdType second) {
		return first.getRaw() < second.getRaw();
	}
};

struct tile_test {
	path testData;
	shared_ptr<Geodata> geo_r;
	shared_ptr<GeodataMock> geo_t;
	tile_test () {
		testData = getInputDirectory() / "karlsruhe_big.carte";
		BOOST_CHECK(boost::filesystem::exists(testData));
		geo_r = boost::make_shared<Geodata>();
		geo_r->load(testData.string());
		geo_t = boost::make_shared<GeodataMock>(*geo_r);
	}

	template<class id_t>
	void compare(const shared_ptr<std::vector<id_t>>& mock, const shared_ptr<std::vector<id_t>>& imp)
	{
		std::sort(mock->begin(), mock->end(), CompareObjects<id_t>());
		std::sort(imp->begin(), imp->end(), CompareObjects<id_t>());

		std::vector<id_t> notInImp;
		std::vector<id_t> notInMock;
		for (auto id : *mock)
		{
			if (!std::binary_search(imp->begin(), imp->end(), id))
				notInImp.push_back(id);
		}
		for (auto id : *imp)
		{
			if (!std::binary_search(mock->begin(), mock->end(), id))
				notInMock.push_back(id);
		}

		std::stringstream ss;
		ss << "Not in implementation: ";
		for (auto id : notInImp)
			ss << " " << id.getRaw() << " ";
		ss << "\n";
		ss << "Not in mock: ";
		for (auto id : notInMock)
			ss << " " << id.getRaw() << " ";
		ss << "\n";

		BOOST_TEST_MESSAGE(ss.str());

		BOOST_CHECK(notInImp.size() == 0 && notInMock.size() == 0 && imp->size() == mock->size());
	}

	void search(int zoom, int x, int y)
	{
		bool nodes_same = true;
		bool ways_same = true;
		bool rels_same = true;
		FixedRect r;
		coord_t x0, y0, x1, y1;
		tileToMercator(x,   y,   zoom, x0, y0);
		tileToMercator(x+1, y+1, zoom, x1, y1);
		r =FixedRect(FixedPoint(x0, y0), FixedPoint(x1, y1));

		/*Nodes*/
		shared_ptr<std::vector<NodeId> > result_t = boost::make_shared< std::vector<NodeId> >();
		shared_ptr<std::vector<NodeId> > result_r = boost::make_shared< std::vector<NodeId> >();
		result_t = geo_t->getNodeIDs(r);
		BOOST_TEST_MESSAGE("Returned nodes-mock: " << result_t->size());
		result_r = geo_r->getNodeIDs(r);
		BOOST_TEST_MESSAGE("Returned nodes: " << result_r->size());
		compare<NodeId>(result_t, result_r);

		/*Ways*/
		shared_ptr<std::vector<WayId> > way_t = boost::make_shared< std::vector<WayId> >();
		shared_ptr<std::vector<WayId> > way_r = boost::make_shared< std::vector<WayId> >();
		way_t = geo_t->getWayIDs(r);
		BOOST_TEST_MESSAGE("Returned ways-mock: " << way_t->size());
		way_r = geo_r->getWayIDs(r);
		BOOST_TEST_MESSAGE("Returned ways: " << way_r->size());
		compare<WayId>(way_t, way_r);

		/*Relation*/
		shared_ptr<std::vector<RelId> > rel_t = boost::make_shared< std::vector<RelId> >();
		shared_ptr<std::vector<RelId> > rel_r = boost::make_shared< std::vector<RelId> >();
		rel_t = geo_t->getRelationIDs(r);
		BOOST_TEST_MESSAGE("Returned relations-mock: " << rel_t->size());
		rel_r = geo_r->getRelationIDs(r);
		BOOST_TEST_MESSAGE("Returned relations: " << rel_r->size());
		compare<RelId>(rel_t, rel_r);
	}
};

ALAC_START_FIXTURE_TEST(tile_test)
	ALAC_FIXTURE_TEST(search,16,34297,22501);
	ALAC_FIXTURE_TEST(search,16,34297,22504);
	ALAC_FIXTURE_TEST(search,15,17150,11251);
	ALAC_FIXTURE_TEST(search,14,8574,5625);
	ALAC_FIXTURE_TEST(search,12,2143,1406);
	ALAC_FIXTURE_TEST(search,13, 4287, 2812);
	ALAC_FIXTURE_TEST(search,11,1071,703);
	ALAC_FIXTURE_TEST(search,10,535,351);
	ALAC_FIXTURE_TEST(search,9,267,175);
	ALAC_FIXTURE_TEST(search,8,133,87);
	ALAC_FIXTURE_TEST(search,7,66,43);
	ALAC_FIXTURE_TEST(search,0,0,0);
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
