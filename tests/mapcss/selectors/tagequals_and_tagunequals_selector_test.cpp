
#include "../../tests.hpp"
#include "server/selectors/tagequals_selector.hpp"
#include "server/selectors/tagunequals_selector.hpp"
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

class TagEqualsAndTagUnequalsSelectorTest {
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

	TagEqualsAndTagUnequalsSelectorTest()
		: nodeId0(0)
		, wayId0(0)
		, relId0(0)
		{
		geodata = std::make_shared<MockGeodata>();
		rule = std::make_shared<MockRule> (geodata);
		tileId = std::make_shared<TileIdentifier> (0, 0, 0, "default", TileIdentifier::PNG);

		next = std::make_shared<MockSelector>(rule);

		CachedString yesTag("yestag");
		CachedString unsetTag("unsettag");
		tags[yesTag] = precached_yes;

		node = std::make_shared<Node>(location, tags);
		way = std::make_shared<Way>(nodes, tags);
		relation = std::make_shared<Relation>(nodes, nodeRoles, ways, wayRoles, tags);

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

	~TagEqualsAndTagUnequalsSelectorTest() {
	}

	void testYesTagEqualsYes() {
		TagEqualsSelector yesTagEqualsYes(rule, next, "yestag", "yes");
		TagUnequalsSelector yesTagUnequalsYes(rule, next, "yestag", "yes");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		yesTagEqualsYes.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		yesTagEqualsYes.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		yesTagEqualsYes.matchRelation(relId0, tileId, &renderAttributes);


		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		yesTagUnequalsYes.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		yesTagUnequalsYes.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		yesTagUnequalsYes.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testYesTagEqualsNo() {
		TagEqualsSelector yesTagEqualsNo(rule, next, "yestag", "no");
		TagUnequalsSelector yesTagUnequalsNo(rule, next, "yestag", "no");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		yesTagEqualsNo.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		yesTagEqualsNo.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		yesTagEqualsNo.matchRelation(relId0, tileId, &renderAttributes);


		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		yesTagUnequalsNo.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		yesTagUnequalsNo.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		yesTagUnequalsNo.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testUnsetTagEqualsYes() {
		TagEqualsSelector unsetTagEqualsYes(rule, next, "unsettag", "yes");
		TagUnequalsSelector unsetTagUnequalsYes(rule, next, "unsettag", "yes");

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		unsetTagEqualsYes.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		unsetTagEqualsYes.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		unsetTagEqualsYes.matchRelation(relId0, tileId, &renderAttributes);


		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		unsetTagUnequalsYes.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		unsetTagUnequalsYes.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		unsetTagUnequalsYes.matchRelation(relId0, tileId, &renderAttributes);
	}

};

ALAC_START_FIXTURE_TEST(TagEqualsAndTagUnequalsSelectorTest)
// functionname, name of test, arguments of function...
ALAC_FIXTURE_TEST_NAMED(testYesTagEqualsYes, YesTagEqualsYesTest)
ALAC_FIXTURE_TEST_NAMED(testYesTagEqualsNo, YesTagEqualsNoTest)
ALAC_FIXTURE_TEST_NAMED(testUnsetTagEqualsYes, UnsetTagEqualsYesTest)
ALAC_END_FIXTURE_TEST()
