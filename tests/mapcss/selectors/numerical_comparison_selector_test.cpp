
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

class NumericalComparisonSelectorTest {
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

	NumericalComparisonSelectorTest()
		: nodeId0(0)
		, wayId0(0)
		, relId0(0)
		{
		geodata = boost::make_shared<MockGeodata>();
		rule = boost::make_shared<MockRule> (geodata);
		tileId = boost::make_shared<TileIdentifier> (0, 0, 0, "default", TileIdentifier::PNG);

		next = boost::make_shared<MockSelector>(rule);

		CachedString valueTag("valuetag");
		CachedString value("42");
		CachedString unsetTag("unsettag");
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

	~NumericalComparisonSelectorTest() {
	}

	void testValueTag42() {
		TagSmallerSelector tagSmallerSelector(rule, next, "valuetag", 42);
		TagLargerSelector tagLargerSelector(rule, next, "valuetag", 42);
		TagSmallerEqualsSelector tagSmallerEqualsSelector(rule, next, "valuetag", 42);
		TagLargerEqualsSelector tagLargerEqualsSelector(rule, next, "valuetag", 42);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagSmallerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagSmallerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagSmallerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testValueTag41() {
		TagSmallerSelector tagSmallerSelector(rule, next, "valuetag", 41);
		TagLargerSelector tagLargerSelector(rule, next, "valuetag", 41);
		TagSmallerEqualsSelector tagSmallerEqualsSelector(rule, next, "valuetag", 41);
		TagLargerEqualsSelector tagLargerEqualsSelector(rule, next, "valuetag", 41);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testValueTag43() {
		TagSmallerSelector tagSmallerSelector(rule, next, "valuetag", 43);
		TagLargerSelector tagLargerSelector(rule, next, "valuetag", 43);
		TagSmallerEqualsSelector tagSmallerEqualsSelector(rule, next, "valuetag", 43);
		TagLargerEqualsSelector tagLargerEqualsSelector(rule, next, "valuetag", 43);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagSmallerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagSmallerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagSmallerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagSmallerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagSmallerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagSmallerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagLargerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagLargerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagLargerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testValueTag1() {
		TagSmallerSelector tagSmallerSelector(rule, next, "valuetag", 1);
		TagLargerSelector tagLargerSelector(rule, next, "valuetag", 1);
		TagSmallerEqualsSelector tagSmallerEqualsSelector(rule, next, "valuetag", 1);
		TagLargerEqualsSelector tagLargerEqualsSelector(rule, next, "valuetag", 1);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testValueTagNeg1() {
		TagSmallerSelector tagSmallerSelector(rule, next, "valuetag", -1);
		TagLargerSelector tagLargerSelector(rule, next, "valuetag", -1);
		TagSmallerEqualsSelector tagSmallerEqualsSelector(rule, next, "valuetag", -1);
		TagLargerEqualsSelector tagLargerEqualsSelector(rule, next, "valuetag", -1);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testValueTag0() {
		TagSmallerSelector tagSmallerSelector(rule, next, "valuetag", 0);
		TagLargerSelector tagLargerSelector(rule, next, "valuetag", 0);
		TagSmallerEqualsSelector tagSmallerEqualsSelector(rule, next, "valuetag", 0);
		TagLargerEqualsSelector tagLargerEqualsSelector(rule, next, "valuetag", 0);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(1);
		tagLargerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);
	}

	void testUnsetTag() {
		TagSmallerSelector tagSmallerSelector(rule, next, "unsettag", 42);
		TagLargerSelector tagLargerSelector(rule, next, "unsettag", 42);
		TagSmallerEqualsSelector tagSmallerEqualsSelector(rule, next, "unsettag", 42);
		TagLargerEqualsSelector tagLargerEqualsSelector(rule, next, "unsettag", 42);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagLargerSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagSmallerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchNode(nodeId0, tileId, &renderAttributes)).Times(0);
		tagLargerEqualsSelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes)).Times(0);
		tagLargerEqualsSelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchRelation(relId0, tileId, &renderAttributes)).Times(0);
		tagLargerEqualsSelector.matchRelation(relId0, tileId, &renderAttributes);
	}

};

ALAC_START_FIXTURE_TEST(NumericalComparisonSelectorTest)
// functionname, name of test, arguments of function...
ALAC_FIXTURE_TEST_NAMED(testValueTag42, ValueTag42Test)
ALAC_FIXTURE_TEST_NAMED(testValueTag43, ValueTag43Test)
ALAC_FIXTURE_TEST_NAMED(testValueTag41, ValueTag41Test)
ALAC_FIXTURE_TEST_NAMED(testValueTag1, ValueTag1Test)
ALAC_FIXTURE_TEST_NAMED(testValueTagNeg1, ValueTagNeg1Test)
ALAC_FIXTURE_TEST_NAMED(testValueTag0, ValueTag0Test)
ALAC_FIXTURE_TEST_NAMED(testUnsetTag, UnsetTagTest)
ALAC_END_FIXTURE_TEST()
