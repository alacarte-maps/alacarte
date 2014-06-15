
#include "../../tests.hpp"

#include <fstream>

#include "general/geo_object.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "utils/transform.hpp"

#include "../../shared/compare.hpp"
#include "../../../src/importer/importer.cpp"

CachedString RandomString()
{
	string str;

	int size = rand() % 15 + 1;

	for(int i = 0; i < size; ++i)
	{
		str += ('a' + rand() % 26);
	}

	return CachedString(str);
}

double randPi()
{
	return std::min(std::max((double)rand()/(double)RAND_MAX * M_PI, -M_PI), M_PI);
}

FixedPoint RandomPoint()
{
	return FixedPoint(randPi(), randPi());
}

DataMap<CachedString, CachedString> RandomTagMap()
{
	int size = rand() % 15;
	DataMap<CachedString, CachedString> tags;

	for(int i = 0; i < size; ++i)
		tags[RandomString()] = RandomString();

	return tags;
}

Node RandomNode()
{
	return Node(RandomPoint(), RandomTagMap());
}

template<typename IdType>
std::vector<IdType> RandomIdVector(int numIds)
{
	std::vector<IdType> list;

	for(int i = 0; i < numIds; ++i)
		list.push_back(IdType(i));

	std::random_shuffle(list.begin(), list.end());

	int size = rand() % numIds + 1;
	list.resize(size);

	return list;
}

Way RandomWay(int numNodes)
{
	return Way(RandomIdVector<NodeId>(numNodes), RandomTagMap());
}

template<typename IdType>
DataMap<IdType, CachedString> RandomRoleMap(const std::vector<IdType>& ids)
{
	DataMap<IdType, CachedString> roles;

	for(auto it = ids.begin(); it != ids.end(); ++it)
		roles[*it] = RandomString();

	return roles;
}

Relation RandomRelation(int numNodes, int numWays)
{
	std::vector<NodeId> nodeRefs(RandomIdVector<NodeId>(numNodes));
	std::vector<WayId> wayRefs(RandomIdVector<WayId>(numWays));

	return Relation(nodeRefs, RandomRoleMap(nodeRefs), wayRefs, RandomRoleMap(wayRefs) , RandomTagMap());
}

void PrintTags(std::ofstream& xml, const DataMap<CachedString, CachedString>& map)
{
	for(auto it = map.begin(); it != map.end(); ++it)
	{
		xml << "\t\t<tag k=\"" << it->first << "\" v=\"" << it->second << "\"/>\n";
	}
}

struct ValidatingFixture
{
	ValidatingFixture(int seed, const string& tempFile)
		: path(getOutputDirectory() / "importer" / tempFile)
	{
		srand(seed);

		nodes = boost::make_shared<std::vector<Node> >();
		ways = boost::make_shared<std::vector<Way> >();
		relations = boost::make_shared<std::vector<Relation> >();

		std::vector<NodeId> nodeIds;
		std::vector<WayId> wayIds;
		std::vector<RelId> relationIds;


		int nodeNum = rand() % 20 + 1;
		for(int i = 0; i < nodeNum; ++i)
		{
			nodes->push_back(RandomNode());
			nodeIds.push_back(NodeId(rand()));
		}


		int wayNum = rand() % 15 + 1;
		for(int i = 0; i < wayNum; ++i)
		{
			ways->push_back(RandomWay(nodeNum));
			wayIds.push_back(WayId(rand()));
		}


		int relNum = rand() % 10 + 1;
		for(int i = 0; i < relNum; ++i)
		{
			relations->push_back(RandomRelation(nodeNum, wayNum));
			relationIds.push_back(RelId(rand()));
		}

		std::ofstream xml(path.string(), std::ios::out);

		BOOST_REQUIRE(xml.is_open());

		xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		xml << "<!-- Random generated osm xml with seed \"" << seed << "\"-->\n";
		xml << "<osm>\n";

		int id = 0;
		for(auto it = nodes->begin(); it != nodes->end(); ++it)
		{
			double lon,lat;
			inverseMercator(it->getLocation(), lat, lon);
			xml << "\t<node id=\"" << nodeIds[id].getRaw() << "\" lon=\"" << lon << "\" lat=\"" << lat << "\">\n";
			PrintTags(xml, it->getTags());
			xml << "\t</node>\n";
			++id;
		}

		id = 0;
		for(auto it = ways->begin(); it != ways->end(); ++it)
		{
			xml << "\t<way id=\"" << wayIds[id].getRaw() << "\">\n";

			for(auto jt = it->getNodeIDs().begin(); jt != it->getNodeIDs().end(); ++jt)
			{
				xml << "\t\t<nd ref=\"" << nodeIds[jt->getRaw()].getRaw() << "\"/>\n";
			}

			PrintTags(xml, it->getTags());
			xml << "\t</way>\n";
			++id;
		}

		id = 0;
		for(auto it = relations->begin(); it != relations->end(); ++it)
		{
			xml << "\t<relation id=\"" << relationIds[id].getRaw() << "\">\n";

			for(auto jt = it->getNodeIDs().begin(); jt != it->getNodeIDs().end(); ++jt)
			{
				xml << "\t\t<member type=\"node\" ref=\"" << nodeIds[jt->getRaw()].getRaw() << "\" role=\"" << it->getNodeRole(*jt) << "\"/>\n";
			}

			for(auto jt = it->getWayIDs().begin(); jt != it->getWayIDs().end(); ++jt)
			{
				xml << "\t\t<member type=\"way\" ref=\"" << wayIds[jt->getRaw()].getRaw() << "\" role=\"" << it->getWayRole(*jt) << "\"/>\n";
			}

			PrintTags(xml, it->getTags());
			xml << "\t</relation>\n";
			++id;
		}

		xml << "</osm>";
		xml.close();
	}

	~ValidatingFixture()
	{
		//std::remove(path.c_str());
	}

	template<typename T1, typename T2>
	void checkMap(const DataMap<T1, T2>& map1, const DataMap<T1, T2>& map2)
	{
		auto it1 = map1.cbegin();
		auto it2 = map2.cbegin();

		for(auto& e : map1)
		{
			auto it = map2.find(e.first);

			BOOST_REQUIRE(it != map2.end());
			BOOST_CHECK(it->second == e.second);
		}
	}

	template<typename IdType>
	void checkVectorSorted(std::vector<IdType> v1, std::vector<IdType> v2)
	{
		std::sort(v1.begin(), v1.end());
		std::sort(v2.begin(), v2.end());
		BOOST_CHECK_EQUAL(v1.size(), v2.size());
		BOOST_CHECK_EQUAL_COLLECTIONS(v1.begin(), v1.end(), v2.begin(), v2.end());
	}

	void checkNodes(const Node& n1, const Node& n2)
	{
		BOOST_CHECK(n1.getLocation().isInRange(n2.getLocation()));
		checkMap(n1.getTags(), n2.getTags());
	}

	void checkWays(const Way& w1, const Way& w2)
	{
		checkVectorSorted(w1.getNodeIDs(), w2.getNodeIDs());
		checkMap(w1.getTags(), w2.getTags());
	}

	void checkRelations(const Relation& r1, const Relation& r2)
	{
		checkVectorSorted(r1.getNodeIDs(), r2.getNodeIDs());
		checkVectorSorted(r1.getWayIDs(), r2.getWayIDs());
		checkMap(r1.getTags(), r2.getTags());

	}

	void validateImporter()
	{
		Importer::OsmXmlParser parser(false);

		parser.parse(path);

		auto parsedNodes = parser.getParsedNodes();
		BOOST_CHECK_EQUAL(nodes->size(), parsedNodes->size());
		for(unsigned int i = 0; i < nodes->size(); ++i)
		{
			checkNodes(nodes->at(i), parsedNodes->at(i));
		}

		auto parsedWays = parser.getParsedWays();
		BOOST_CHECK_EQUAL(ways->size(), parsedWays->size());
		for(unsigned int i = 0; i < ways->size(); ++i)
		{
			checkWays(ways->at(i), parsedWays->at(i));
		}

		auto parsedRelations = parser.getParsedRelations();
		BOOST_CHECK_EQUAL(relations->size(), parsedRelations->size());
		for(unsigned int i = 0; i < relations->size(); ++i)
		{
			checkRelations(relations->at(i), parsedRelations->at(i));
		}
	}


	const boost::filesystem::path path;


	std::shared_ptr< std::vector<Node> > nodes;
	std::shared_ptr< std::vector<Way> > ways;
	std::shared_ptr< std::vector<Relation> > relations;
};

ALAC_START_FIXTURE_TEST(ValidatingFixture, 100, "tmp1.xml")
	ALAC_FIXTURE_TEST(validateImporter);
ALAC_END_FIXTURE_TEST();

ALAC_START_FIXTURE_TEST(ValidatingFixture, 200, "tmp2.xml")
	ALAC_FIXTURE_TEST(validateImporter);
ALAC_END_FIXTURE_TEST();

ALAC_START_FIXTURE_TEST(ValidatingFixture, 300, "tmp3.xml")
	ALAC_FIXTURE_TEST(validateImporter);
ALAC_END_FIXTURE_TEST();

ALAC_START_FIXTURE_TEST(ValidatingFixture, 400, "tmp4.xml")
	ALAC_FIXTURE_TEST(validateImporter);
ALAC_END_FIXTURE_TEST();

ALAC_START_FIXTURE_TEST(ValidatingFixture, 500, "tmp5.xml")
	ALAC_FIXTURE_TEST(validateImporter);
ALAC_END_FIXTURE_TEST();
