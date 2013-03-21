#include "includes.hpp"
#include "settings.hpp"

#include "server/render_attributes.hpp"
#include "server/style.hpp"

#include <gmock/gmock.h>

typedef boost::unordered_map<NodeId, Style*> nodestylemap;
typedef boost::unordered_map<WayId, Style*> waystylemap;
typedef boost::unordered_map<RelId, Style*> relstylemap;

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::Matcher;

class MockRenderAttributes : public RenderAttributes {
private:
	RenderAttributes real_;
public:
	MockRenderAttributes() : RenderAttributes() {
		ON_CALL(*this, getNodeMap())
		.WillByDefault(Invoke(&real_, &RenderAttributes::getNodeMap));
		ON_CALL(*this, getWayMap())
		.WillByDefault(Invoke(&real_, &RenderAttributes::getWayMap));
		ON_CALL(*this, getRelationMap())
		.WillByDefault(Invoke(&real_, &RenderAttributes::getRelationMap));

		ON_CALL(*this, getCanvasStyle())
		.WillByDefault(Invoke(&real_, &RenderAttributes::getCanvasStyle));

		typedef Style* (RenderAttributes::*newNodeStyle)(NodeId);
		typedef Style* (RenderAttributes::*newWayStyle)(WayId);
		typedef Style* (RenderAttributes::*newRelationStyle)(RelId);

		ON_CALL(*this, getNewStyle(Matcher<NodeId>(_)))
		.WillByDefault(Invoke(&real_, (newNodeStyle)&RenderAttributes::getNewStyle));
		ON_CALL(*this, getNewStyle(Matcher<WayId>(_)))
		.WillByDefault(Invoke(&real_, (newWayStyle)&RenderAttributes::getNewStyle));
		ON_CALL(*this, getNewStyle(Matcher<RelId>(_)))
		.WillByDefault(Invoke(&real_, (newRelationStyle)&RenderAttributes::getNewStyle));
	}

	MOCK_CONST_METHOD0(getNodeMap, const nodestylemap&());
	MOCK_CONST_METHOD0(getWayMap, const waystylemap&());
	MOCK_CONST_METHOD0(getRelationMap, const relstylemap&());
	MOCK_METHOD0(getCanvasStyle, Style*());

	MOCK_METHOD1(getNewStyle, Style*(NodeId id));
	MOCK_METHOD1(getNewStyle, Style*(RelId id));
	MOCK_METHOD1(getNewStyle, Style*(WayId id));

};
