
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
#include "utils/precached_strings.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::AnyNumber;

class HasTagAndHasNotTagSelectorTest {
public:
	std::shared_ptr<MockGeodata> geodata;
	std::shared_ptr<MockRule> rule;
	std::shared_ptr<TileIdentifier> tileId;
	std::shared_ptr<MockSelector> next;
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

	std::shared_ptr<Node> node;
	std::shared_ptr<Way> way;
	std::shared_ptr<Relation> relation;

	HasTagAndHasNotTagSelectorTest()
		: nodeId0(0)
		, wayId0(0)
		, relId0(0)
		{
		geodata = boost::make_shared<MockGeodata>();
		rule = boost::make_shared<MockRule> (geodata);
		tileId = boost::make_shared<TileIdentifier> (0, 0, 0, "default", TileIdentifier::PNG);

		next = boost::make_shared<MockSelector>(rule);

		CachedString yesTag("yestag");
		CachedString noTag("notag");
		CachedString falseTag("falsetag");
		CachedString unsetTag("unsettag");
		CachedString valueTag("valuetag");
		CachedString value("value");
		tags[yesTag] = precached_yes;
		tags[noTag] = precached_no;
		tags[falseTag] = precached_false;
		tags[valueTag] = value;

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

	~HasTagAndHasNotTagSelectorTest() {
	}

	void testYesTag() {
		HasTagSelector hasYesTag(rule, next, "yestag");
		HasNotTagSelector hasNotYesTag(rule, next, "yestag");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		hasYesTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		hasNotYesTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		hasYesTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		hasNotYesTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		hasYesTag.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		hasNotYesTag.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testNoTag() {
		HasTagSelector hasNoTag = HasTagSelector(rule, next, "notag");
		HasNotTagSelector hasNotNoTag = HasNotTagSelector(rule, next, "notag");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		hasNoTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		hasNotNoTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		hasNoTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		hasNotNoTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		hasNoTag.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		hasNotNoTag.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testFalseTag() {
		HasTagSelector hasFalseTag = HasTagSelector(rule, next, "falsetag");
		HasNotTagSelector hasNotFalseTag = HasNotTagSelector(rule, next, "falsetag");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		hasFalseTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		hasNotFalseTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		hasFalseTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		hasNotFalseTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		hasFalseTag.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		hasNotFalseTag.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testUnsetTag() {
		HasTagSelector hasUnsetTag = HasTagSelector(rule, next, "unsettag");
		HasNotTagSelector hasNotUnsetTag = HasNotTagSelector(rule, next, "unsettag");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		hasUnsetTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		hasNotUnsetTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		hasUnsetTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		hasNotUnsetTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		hasUnsetTag.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		hasNotUnsetTag.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testValueTag() {
		HasTagSelector hasValueTag = HasTagSelector(rule, next, "valuetag");
		HasNotTagSelector hasNotValueTag = HasNotTagSelector(rule, next, "valuetag");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		hasValueTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		hasNotValueTag.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		hasValueTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		hasNotValueTag.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		hasValueTag.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		hasNotValueTag.matchRelation(relId0, tileId, &renderAttributes);
	}

};

ALAC_START_FIXTURE_TEST(HasTagAndHasNotTagSelectorTest)
// functionname, name of test, arguments of function...
ALAC_FIXTURE_TEST_NAMED(testYesTag, YesTagTest)
ALAC_FIXTURE_TEST_NAMED(testNoTag, NoTagTest)
ALAC_FIXTURE_TEST_NAMED(testFalseTag, FalseTagTest)
ALAC_FIXTURE_TEST_NAMED(testUnsetTag, UnsetTagTest)
ALAC_FIXTURE_TEST_NAMED(testValueTag, ValueTagTest)
ALAC_END_FIXTURE_TEST()
