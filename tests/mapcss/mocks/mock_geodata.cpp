#include "includes.hpp"
#include "settings.hpp"

#include "general/geodata.hpp"

#include <gmock/gmock.h>

class MockGeodata : public Geodata {
public:
	MockGeodata() : Geodata() {}

	MOCK_CONST_METHOD1(getNode, Node*(NodeId));
	MOCK_CONST_METHOD1(getWay, Way*(WayId));
	MOCK_CONST_METHOD1(getRelation, Relation*(RelId));
};