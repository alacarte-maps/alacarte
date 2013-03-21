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
#include "utils/precached_strings.hpp"

using ::testing::_;
using ::testing::Return;

class AreaAndLineSelectorTest {
public:
	shared_ptr<MockGeodata> geodata;
	shared_ptr<MockRule> rule;
	shared_ptr<TileIdentifier> tileId;
	shared_ptr<MockSelector> next;
	MockRenderAttributes renderAttributes;
	shared_ptr<AreaSelector> areaSelector;
	shared_ptr<LineSelector> lineSelector;

	std::vector<NodeId> openWayNodes;
	std::vector<NodeId> closedWayNodes;


	DataMap<CachedString, CachedString> areaUnset;
	DataMap<CachedString, CachedString> areaYes;
	DataMap<CachedString, CachedString> areaNo;

	NodeId nodeId0;
	WayId wayId0;
	RelId relId0;

	AreaAndLineSelectorTest() : nodeId0(0), wayId0(0), relId0(0) {
		geodata = boost::make_shared<MockGeodata>();
		rule = boost::make_shared<MockRule> (geodata);
		tileId = boost::make_shared<TileIdentifier> (0, 0, 0, "default", TileIdentifier::PNG);
		next = boost::make_shared<MockSelector>(rule);

		areaSelector = boost::make_shared<AreaSelector> (rule, next);
		lineSelector = boost::make_shared<LineSelector> (rule, next);

		openWayNodes.push_back(nodeId0);
		openWayNodes.push_back(NodeId(1));

		closedWayNodes.push_back(nodeId0);
		closedWayNodes.push_back(NodeId(1));
		closedWayNodes.push_back(nodeId0);

		areaYes[precached_area] = precached_yes;
		areaNo[precached_area] = precached_no;
	}

	~AreaAndLineSelectorTest() {
	}

	/*
	Area
	offen und area ungesetzt:        - Nein
	geschlossen und area ungesetzt:  - Ja
	offen und area = yes:            - Nein (invalide Daten)
	geschlossen und area = yes:      - Ja
	offen und area = no:             - Nein
	geschlossen und area = no:       - Nein

	Line
	offen und area ungesetzt:        - Ja
	geschlossen und area ungesetzt:  - Nein
	offen und area = yes:            - Ja (Fallback wenn invalide Daten da sind)
	geschlossen und area = yes:      - Nein
	offen und area = no:             - Ja
	geschlossen und area = no:       - Ja
	*/

	void testOpenWay() {
		Way openWay(openWayNodes, areaUnset);

		EXPECT_CALL(*geodata, getWay(wayId0))
		.Times(2)
		.WillRepeatedly(Return(&openWay));

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(0);
		areaSelector->matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(1);
		lineSelector->matchWay(wayId0, tileId, &renderAttributes);
	}

	void testClosedWay() {
		Way closedWay(closedWayNodes, areaUnset);

		EXPECT_CALL(*geodata, getWay(wayId0))
		.Times(2)
		.WillRepeatedly(Return(&closedWay));

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(1);
		areaSelector->matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(0);
		lineSelector->matchWay(wayId0, tileId, &renderAttributes);
	}

	void testOpenArea() {
		// open Area => invalid data
		Way openArea(openWayNodes, areaYes);

		EXPECT_CALL(*geodata, getWay(wayId0))
		.Times(2)
		.WillRepeatedly(Return(&openArea));

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(0);
		areaSelector->matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(1);
		lineSelector->matchWay(wayId0, tileId, &renderAttributes);
	}

	void testClosedArea() {
		Way closedArea(closedWayNodes, areaYes);

		EXPECT_CALL(*geodata, getWay(wayId0))
		.Times(2)
		.WillRepeatedly(Return(&closedArea));

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(1);
		areaSelector->matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(0);
		lineSelector->matchWay(wayId0, tileId, &renderAttributes);

	}

	void testOpenLine() {
		Way openLine(openWayNodes, areaNo);

		EXPECT_CALL(*geodata, getWay(wayId0))
		.Times(2)
		.WillRepeatedly(Return(&openLine));

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(0);
		areaSelector->matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(1);
		lineSelector->matchWay(wayId0, tileId, &renderAttributes);

	}

	void testClosedLine() {
		Way closedLine(closedWayNodes, areaNo);

		EXPECT_CALL(*geodata, getWay(wayId0))
		.Times(2)
		.WillRepeatedly(Return(&closedLine));

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(0);
		areaSelector->matchWay(wayId0, tileId, &renderAttributes);

		EXPECT_CALL(*next, matchWay(wayId0, tileId, &renderAttributes))
		.Times(1);
		lineSelector->matchWay(wayId0, tileId, &renderAttributes);

	}

	void testNoForward() {
		EXPECT_CALL(*next, matchNode(_, _, _)).Times(0);
		EXPECT_CALL(*next, matchRelation(_, _, _)).Times(0);

		lineSelector->matchNode(nodeId0, tileId, &renderAttributes);
		lineSelector->matchRelation(relId0, tileId, &renderAttributes);

		areaSelector->matchNode(nodeId0, tileId, &renderAttributes);
		areaSelector->matchRelation(relId0, tileId, &renderAttributes);
	}

};

ALAC_START_FIXTURE_TEST(AreaAndLineSelectorTest)

// functionname, name of test, arguments of function...
ALAC_FIXTURE_TEST_NAMED(testOpenWay, OpenWayTest)
ALAC_FIXTURE_TEST_NAMED(testClosedWay, ClosedWayTest)
ALAC_FIXTURE_TEST_NAMED(testOpenArea, OpenAreaTest)
ALAC_FIXTURE_TEST_NAMED(testClosedArea, ClosedAreaTest)
ALAC_FIXTURE_TEST_NAMED(testOpenLine, OpenLineTest)
ALAC_FIXTURE_TEST_NAMED(testClosedLine, ClosedLineTest)
ALAC_FIXTURE_TEST_NAMED(testNoForward, NoForwardTest)

ALAC_END_FIXTURE_TEST()