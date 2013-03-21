#include "includes.hpp"
#include "../../tests.hpp"
#include "server/selectors/tagmatches_selector.hpp"
#include "general/geodata.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "../mocks/mock_selector.cpp"
#include "../mocks/mock_rule.cpp"
#include "../mocks/mock_geodata.cpp"
#include "../mocks/mock_render_attributes.cpp"
#include "utils/precached_strings.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::AnyNumber;

class TagMatchesSelectorTest {
public:
	shared_ptr<MockGeodata> geodata;
	shared_ptr<MockRule> rule;
	shared_ptr<TileIdentifier> tileId;
	shared_ptr<MockSelector> next;
	MockRenderAttributes renderAttributes;

	NodeId nodeId0;
	WayId wayId0;
	RelId relId0;

	DataMap<CachedString, CachedString> tags;

	FloatPoint location;
	std::vector<NodeId> nodes;
	std::vector<WayId> ways;
	DataMap<NodeId, CachedString> nodeRoles;
	DataMap<WayId, CachedString> wayRoles;

	shared_ptr<Node> node;
	shared_ptr<Way> way;
	shared_ptr<Relation> relation;

	TagMatchesSelectorTest()
	: nodeId0(0)
	, wayId0(0)
	, relId0(0)
	{
		geodata = boost::make_shared<MockGeodata>();
		rule = boost::make_shared<MockRule> (geodata);
		tileId = boost::make_shared<TileIdentifier> (0, 0, 0, "default", TileIdentifier::PNG);

		next = boost::make_shared<MockSelector>(rule);

		CachedString primaryTag("primarytag");
		CachedString secondaryTag("secondarytag");
		CachedString blubTag("blubtag");
		CachedString unsetTag("unsettag");
		CachedString primaryValue("primary");
		CachedString secondaryValue("secondary");
		CachedString blubValue("blub");

		tags[primaryTag] = primaryValue;
		tags[secondaryTag] = secondaryValue;
		tags[blubTag] = blubValue;

		node = boost::make_shared<Node>(location, tags);
		way = boost::make_shared<Way>(nodes, tags);
		relation = boost::make_shared<Relation>(nodes, nodeRoles, ways, wayRoles, tags);

		EXPECT_CALL(*geodata, getNode(nodeId0))
		.Times(AnyNumber())
		.WillRepeatedly(Return(&(*node)));
		EXPECT_CALL(*geodata, getWay(wayId0))
		.Times(AnyNumber())
		.WillRepeatedly(Return(&(*way)));
		EXPECT_CALL(*geodata, getRelation(relId0))
		.Times(AnyNumber())
		.WillRepeatedly(Return(&(*relation)));
	}

	~TagMatchesSelectorTest() {
	}

	void testPrimaryTagMatches() {
		TagMatchesSelector primaryTagMatches(rule, next, "primarytag", ".*ary");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		primaryTagMatches.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		primaryTagMatches.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		primaryTagMatches.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testSecondaryTagMatches() {
		TagMatchesSelector secondaryTagMatches(rule, next, "secondarytag", ".*ary");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		secondaryTagMatches.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		secondaryTagMatches.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		secondaryTagMatches.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testBlubTagMatches() {
		TagMatchesSelector blubTagMatches(rule, next, "blubtag", ".*ary");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		blubTagMatches.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		blubTagMatches.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		blubTagMatches.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testUnsetTagMatches() {
		TagMatchesSelector unsetTagMatches(rule, next, "unsettag", ".*ary");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		unsetTagMatches.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		unsetTagMatches.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		unsetTagMatches.matchRelation(relId0, tileId, &renderAttributes);
	}

};

ALAC_START_FIXTURE_TEST(TagMatchesSelectorTest)
// functionname, name of test, arguments of function...
ALAC_FIXTURE_TEST_NAMED(testPrimaryTagMatches, PrimaryTagMatchesTest)
ALAC_FIXTURE_TEST_NAMED(testSecondaryTagMatches, SecondaryTagMatchesTest)
ALAC_FIXTURE_TEST_NAMED(testBlubTagMatches, BlubTagMatchesTest)
ALAC_FIXTURE_TEST_NAMED(testUnsetTagMatches, UnsetTagMatchesTest)
ALAC_END_FIXTURE_TEST()