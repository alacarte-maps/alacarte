#include "includes.hpp"
#include "../../tests.hpp"
#include "server/selectors/selectors.hpp"
#include "general/geodata.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "../mocks/mock_selector.cpp"
#include "../mocks/mock_rule.cpp"
#include "../mocks/mock_geodata.cpp"
#include "../mocks/mock_render_attributes.cpp"

using ::testing::_;
using ::testing::Return;
using ::testing::InSequence;
using ::testing::AnyNumber;

class ChildNodesAndChildWaysSelectorTest {
public:
	shared_ptr<MockGeodata> geodata;
	shared_ptr<MockRule> rule;
	shared_ptr<TileIdentifier> tileId;
	shared_ptr<MockSelector> next;
	MockRenderAttributes renderAttributes;
	shared_ptr<ChildNodesSelector> childNodesSelector;
	shared_ptr<ChildWaysSelector> childWaysSelector;

	DataMap<CachedString, CachedString> tags;
	std::vector<NodeId> nodes;
	std::vector<WayId> ways;
	DataMap<NodeId, CachedString> nodeRoles;
	DataMap<WayId, CachedString> wayRoles;
	shared_ptr<Way> way;
	shared_ptr<Relation> relation;

	NodeId nodeId0;
	WayId wayId0;
	RelId relId0;

	// setup
	ChildNodesAndChildWaysSelectorTest() : nodeId0(0), wayId0(0), relId0(0) {
		geodata = boost::make_shared<MockGeodata>();
		rule = boost::make_shared<MockRule> (geodata);
		tileId = boost::make_shared<TileIdentifier> (0, 0, 0, "default", TileIdentifier::PNG);
		next = boost::make_shared<MockSelector>(rule);

		nodes.push_back(nodeId0);
		nodes.push_back(NodeId(1));
		nodes.push_back(NodeId(2));

		ways.push_back(wayId0);

		way = boost::make_shared<Way>(nodes, tags);
		relation = boost::make_shared<Relation>(nodes, nodeRoles, ways, wayRoles, tags);

		childNodesSelector = boost::make_shared<ChildNodesSelector> (rule, next);
		childWaysSelector = boost::make_shared<ChildWaysSelector> (rule, next);

		EXPECT_CALL(*geodata, getWay(wayId0))
		.Times(AnyNumber())
		.WillRepeatedly(Return(&(*way)));

		EXPECT_CALL(*geodata, getRelation(relId0))
		.Times(AnyNumber())
		.WillRepeatedly(Return(&(*relation)));
	}

	// teardown
	~ChildNodesAndChildWaysSelectorTest() {
	}

	void testWayChildNodes() {
		{
			InSequence dummy;
			EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
			EXPECT_CALL(*next, matchNode(NodeId(1), tileId, &renderAttributes)).Times(1);
			EXPECT_CALL(*next, matchNode(NodeId(2), tileId, &renderAttributes)).Times(1);
		}
		childNodesSelector->matchWay(wayId0, tileId, &renderAttributes);
	}

	void testRelationChildNodes() {
		{
			InSequence dummy;
			EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
			EXPECT_CALL(*next, matchNode(NodeId(1), tileId, &renderAttributes)).Times(1);
			EXPECT_CALL(*next, matchNode(NodeId(2), tileId, &renderAttributes)).Times(1);
		}
		childNodesSelector->matchRelation(relId0, tileId, &renderAttributes);
	}

	void testRelationChildWays() {
		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		childWaysSelector->matchRelation(relId0, tileId, &renderAttributes);
	}

	void testNoForward() {
		EXPECT_CALL(*next, matchNode(_, _, _)).Times(0);
		EXPECT_CALL(*next, matchWay(_, _, _)).Times(0);
		EXPECT_CALL(*next, matchRelation(_, _, _)).Times(0);

		childNodesSelector->matchNode(nodeId0, tileId, &renderAttributes);
		childWaysSelector->matchNode(nodeId0, tileId, &renderAttributes);
		childWaysSelector->matchWay(wayId0, tileId, &renderAttributes);
	}

};

ALAC_START_FIXTURE_TEST(ChildNodesAndChildWaysSelectorTest)

// functionname, name of test, arguments of function...
ALAC_FIXTURE_TEST_NAMED(testNoForward, NoForwardTest)
ALAC_FIXTURE_TEST_NAMED(testWayChildNodes, WayChildNodesTest)
ALAC_FIXTURE_TEST_NAMED(testRelationChildNodes, RelationChildNodesTest)
ALAC_FIXTURE_TEST_NAMED(testRelationChildWays, RelationChildWaysTest)

ALAC_END_FIXTURE_TEST()
