
#include "settings.hpp"

#include "server/rule.hpp"

#include <gmock/gmock.h>

class MockRule : public Rule {
public:
	MockRule(const std::shared_ptr<Geodata> geodata) : Rule(geodata) {}

	MOCK_CONST_METHOD5(match, void(const std::shared_ptr<std::vector<NodeId> >& nodeIDs,
									const std::shared_ptr<std::vector<WayId> >& wayIDs,
									const std::shared_ptr<std::vector<RelId> >& relIDs,
									const std::shared_ptr<TileIdentifier>& ti,
									RenderAttributes* renderAttributes));

	MOCK_CONST_METHOD0(getStyleTemplate, const std::shared_ptr<StyleTemplate>&());
};
