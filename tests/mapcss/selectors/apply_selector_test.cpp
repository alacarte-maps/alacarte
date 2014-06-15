
#include "../../tests.hpp"
#include "server/selectors/apply_selector.hpp"
#include "server/style_template.hpp"
#include "general/geodata.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "../mocks/mock_rule.cpp"
#include "../mocks/mock_geodata.cpp"
#include "../mocks/mock_render_attributes.cpp"
#include "server/tile_identifier.hpp"
#include "utils/precached_strings.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::AnyNumber;

class ApplySelectorTest {
public:
	std::shared_ptr<MockGeodata> geodata;
	std::shared_ptr<MockRule> rule;
	std::shared_ptr<TileIdentifier> tileId;
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

	std::shared_ptr<StyleTemplate> styleTemplate;

	std::shared_ptr<Node> node;
	std::shared_ptr<Way> way;
	std::shared_ptr<Relation> relation;

	ApplySelectorTest()
	: nodeId0(0)
	, wayId0(0)
	, relId0(0)
	{
		geodata = boost::make_shared<MockGeodata>();
		rule = boost::make_shared<MockRule>(geodata);
		tileId = boost::make_shared<TileIdentifier> (0, 0, 0, "default", TileIdentifier::PNG);

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

		styleTemplate = boost::make_shared<StyleTemplate>();

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

		EXPECT_CALL(*rule, getStyleTemplate())
		.Times(AnyNumber())
		.WillRepeatedly(ReturnRef(styleTemplate));
	}

	~ApplySelectorTest() {
	}

	void testApplySelector() {
		ApplySelector applySelector(rule);

		EXPECT_CALL(renderAttributes, getNodeMap()).Times(2);
		EXPECT_CALL(renderAttributes, getNewStyle(nodeId0)).Times(1);
		applySelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(renderAttributes, getWayMap()).Times(2);
		EXPECT_CALL(renderAttributes, getNewStyle(wayId0)).Times(1);
		applySelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(renderAttributes, getRelationMap()).Times(2);
		EXPECT_CALL(renderAttributes, getNewStyle(relId0)).Times(1);
		applySelector.matchRelation(relId0, tileId, &renderAttributes);


		EXPECT_CALL(renderAttributes, getNodeMap()).Times(2);
		EXPECT_CALL(renderAttributes, getNewStyle(nodeId0)).Times(0);
		applySelector.matchNode(nodeId0, tileId, &renderAttributes);

		EXPECT_CALL(renderAttributes, getWayMap()).Times(2);
		EXPECT_CALL(renderAttributes, getNewStyle(wayId0)).Times(0);
		applySelector.matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(renderAttributes, getRelationMap()).Times(2);
		EXPECT_CALL(renderAttributes, getNewStyle(relId0)).Times(0);
		applySelector.matchRelation(relId0, tileId, &renderAttributes);
	}

};

ALAC_START_FIXTURE_TEST(ApplySelectorTest)
// functionname, name of test, arguments of function...
ALAC_FIXTURE_TEST_NAMED(testApplySelector, ApplySelectorTest)
ALAC_END_FIXTURE_TEST()
