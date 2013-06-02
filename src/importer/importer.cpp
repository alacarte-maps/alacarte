/**
 *  This file is part of alaCarte.
 *
 *  alaCarte is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  alaCarte is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with alaCarte. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright alaCarte 2012-2013 Simon Dreher, Florian Jacob, Tobias Kahlert, Patrick Niklaus, Bernhard Scheirle, Lisa Winter
 *  Maintainer: Tobias Kahlert
 */



#include <boost/filesystem/path.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem/operations.hpp>
 #include <boost/type_traits/is_same.hpp>

#include "../../extras/eaglexml/eaglexml.hpp"
#include "../../extras/eaglexml/eaglexml_iterators.hpp"

#include "importer/importer.hpp"

#include "general/configuration.hpp"
#include "general/geodata.hpp"

#include "general/geo_object.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"


using boost::filesystem::path;


/**
 * @brief Parser for osm-xml-data                                                                     
 *
 * This parser will parse xml files and extract nodes ways and relations
 **/
class Importer::OsmXmlParser
	: public eaglexml::stream_cache<>::stream_cache_observer
{
public:
	//! flags for parsing the xml file
	static const int parser_flags = eaglexml::parse_no_data_nodes
									| eaglexml::parse_normalize_whitespace
									| eaglexml::parse_trim_whitespace
									| eaglexml::parse_validate_closing_tags
									| eaglexml::parse_default;

	//! Type, where osm ids stored in.
	typedef uint64_t OsmIdType;

public:
	/**
	 * @brief Creates a new parser and sets default settings                                                                     
	 *
	 **/
	OsmXmlParser(bool ignoreUnknownEntities)
		: ignoreUnknownEntities(ignoreUnknownEntities)
		, alreadyRead(0)
		, fileSize(0)
		, segmentSize(1024 * 1024)
		, log(log4cpp::Category::getInstance("Importer"))
		, outputIgnoreRelation(false)
		, outputIgnoreBounds(false)
	{
	}

	/**
	 * @brief Will parse a given xml file.
	 *
	 * This method will try to open the given file. osm data is expected to be found.
	 * nodes, ways and relations will be extracted and saved. They can be accessed through
	 * getParsedNodes, getParsedWays, getParsedRelations. Additionally a mapping from
	 * osm id to an internal id will be created for nodes and ways.
	 *
	 * @param xml_file A path to a file containing the osm data.
	 * @throws FileNotFoundException
	 **/
	void parse(const path& xml_file)
	{
		assert(!nodes);
		assert(!ways);
		assert(!relations);

		std::ifstream xml_stream(xml_file.string());

		if(!xml_stream)
			BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(xml_file.string()));

		log.infoStream() << "Load xml-file \"" << xml_file.string() << "\"";
		
		fileSize = boost::filesystem::file_size(xml_file);

		log.infoStream() << "File size is " << fileSize / (1024) << "kb";

		nodes = boost::make_shared<std::vector<Node> >();
		ways = boost::make_shared<std::vector<Way> >();
		relations = boost::make_shared<std::vector<Relation> >();

		// Use a cache with an 8 mb buffer
		eaglexml::stream_cache<> cache(xml_stream, 8 * 1024 * 1024);
		eaglexml::xml_document<> document;

		cache.segment_size(segmentSize);
		cache.max_segments_to_read(1);
		cache.observer(this);


		try {
			// Init the xml parser
			document.parse<parser_flags>(&cache);

			eaglexml::xml_node<>* osm = document.first_node("osm");

			if(!osm)
				BOOST_THROW_EXCEPTION(excp::InputFormatException() << excp::InfoXmlEntityName("<osm>")  << excp::InfoWhat("Missing \"<osm>\" xml node!"));

			parseEntities(osm);

		}catch(excp::InputFormatException& e) {
			e << excp::InfoFileName(xml_file.string());
			throw;
		}catch(eaglexml::parse_error& e)
		{
			BOOST_THROW_EXCEPTION(excp::InputFormatException() << excp::InfoFileName(xml_file.string()) << excp::InfoWhat(e.what()));
		}

		log.infoStream() << "Precompute attributes...";
		precomputeAttributes();
	}

	/**
	 * @brief Returns a vector with parsed nodes                                                                     
	 *
	 * @return parsed nodes
	 **/
	shared_ptr< std::vector<Node> > getParsedNodes() const
	{
		return nodes;
	}
	
	/**
	 * @brief Returns a vector with parsed ways                                                                     
	 *
	 * @return parsed ways
	 **/
	shared_ptr< std::vector<Way> > getParsedWays() const
	{
		return ways;
	}
	
	/**
	 * @brief Returns a vector with parsed relations                                                                     
	 *
	 * @return parsed relations
	 **/
	shared_ptr< std::vector<Relation> > getParsedRelations() const
	{
		return relations;
	}
	
private:
	/**
	 * @brief parses the osm-root element and creates given objects                                                                     
	 *
	 * @param osm_root osm-xml root node
	 **/
	void parseEntities(eaglexml::xml_node<>* osmRoot)
	{
		assert(osmRoot);

		std::map<std::string, void (OsmXmlParser::*)(eaglexml::xml_node<>*)> entities;

		// Entities that can be parsed
		entities["bounds"] = &OsmXmlParser::parseBounds;
		entities["node"] = &OsmXmlParser::parseNode;
		entities["way"] = &OsmXmlParser::parseWay;
		entities["relation"] = &OsmXmlParser::parseRelation;

		for(eaglexml::node_iterator<> it(osmRoot);
			it != eaglexml::node_iterator<>();
			++it)
		{
			auto entityIt = entities.find(it->name());

			if(!ignoreUnknownEntities && entityIt == entities.end())
				BOOST_THROW_EXCEPTION(excp::InputFormatException() << excp::InfoXmlEntityName(it->name()) << excp::InfoWhat("Unknown entity in xml file!"));

			try{ 
				(this->*(entityIt->second))(&*it);
			}catch(excp::BadOsmIdException& e) {
				log.warnStream() << "Bad osm id[" << *boost::get_error_info<excp::InfoUnresolvableId>(e)  << "]. Entity is skipped!";
			}
		}
	}

	/**
	 * @brief adds additional information to the objects that should not be computed at runtime.
	 *
	 */
	void precomputeAttributes()
	{
		boost::unordered_map<NodeId, unsigned int> references;

		unsigned int count;
		for (auto& way : *ways)
		{
			auto& nodes = way.getNodeIDs();
			count = references[nodes.front()];
			references[nodes.front()] = count+1;
			count = references[nodes.back()];
			references[nodes.back()] = count+1;

			if (nodes.front() == nodes.back())
				way.setType(Way::WayType::CLOSED);
		}

		for (auto& way : *ways)
		{
			auto& nodes = way.getNodeIDs();

			Way::WayType type = way.getType();

			count = references[nodes.front()];
			if (count > 1)
				type = (Way::WayType) (type | Way::WayType::CONNECTED_START);
			count = references[nodes.back()];
			if (count > 1)
				type = (Way::WayType) (type | Way::WayType::CONNECTED_END);

			way.setType(type);
		}
	}
	
	/**
	 * @brief parses the bound entity                                                                   
	 *
	 * @param node xml-node for the entity
	 **/
	void parseBounds(eaglexml::xml_node<>* node)
	{
		if(!outputIgnoreBounds)
		{
			log.infoStream() << "Bounds tag in osm data is ignored by this software!";
			outputIgnoreBounds = true;
		}
	}

	/**
	 * @brief parses a node entity
	 *
	 * @param node xml-node for the entity
	 **/
	void parseNode(eaglexml::xml_node<>* node)
	{
		assert(node);

		OsmIdType id;
		double lon, lat;
		extractAttributeFromNode("id", node, &id);
		extractAttributeFromNode("lon", node, &lon);
		extractAttributeFromNode("lat", node, &lat);



		DataMap<CachedString, CachedString> tags;
		parseProperties<Node>(node->first_node(), &tags, nullptr, nullptr, nullptr, nullptr);

		nodeIdMapping.insert(std::make_pair(id, NodeId(nodes->size())));
		nodes->push_back(Node(FloatPoint(lon, lat), tags));
	}
	
	/**
	 * @brief parses the way entity                                                                   
	 *
	 * @param node xml-node for the entity
	 **/
	void parseWay(eaglexml::xml_node<>* way)
	{
		assert(way);

		OsmIdType id;
		extractAttributeFromNode("id", way, &id);


		DataMap<CachedString, CachedString> tags;
		std::vector<NodeId> nodeIds;

		parseProperties<Way>(way->first_node(), &tags, &nodeIds, nullptr, nullptr, nullptr);

		if (nodeIds.size() == 0)
			return;

		wayIdMapping.insert(std::make_pair(id, WayId(ways->size())));
		ways->push_back(Way(nodeIds, tags));
	}
	
	/**
	 * @brief parses the relation entity                                                                   
	 *
	 * @param node xml-node for the entity
	 **/
	void parseRelation(eaglexml::xml_node<>* relation)
	{
		assert(relation);

		OsmIdType id;
		extractAttributeFromNode("id", relation, &id);

		DataMap<CachedString, CachedString> tags;
		std::vector<NodeId> nodeIds;
		std::vector<WayId> wayIds;
		DataMap<NodeId, CachedString> nodeRoles;
		DataMap<WayId, CachedString> wayRoles;


		parseProperties<Relation>(relation->first_node(), &tags, &nodeIds, &nodeRoles, &wayIds, &wayRoles);

		if (nodeIds.size() == 0 && wayIds.size() == 0)
			return;

		relations->push_back(Relation(nodeIds, nodeRoles, wayIds, wayRoles, tags));
	}

	
	/**
	 * @brief parses properties of an osm-object
	 *
	 * Starts with the first and goes through all property xml-nodes.
	 * Depending on the Target, specified, the nodes will be examined and data will be extracted.
	 *
	 * @tparam Target The geoobject-type for which the properties should be examined. Must be one of the following: Node, Way, Relation
	 *
	 * @param firstProp first xml-property-node
	 * @param tagMap map, where tags are saved
	 * @param nodeRefIds list, where references to nodes are saved
	 * @param nodeRoleMap map, where roles of nodes are saved
	 * @param wayRefIds list, where references to ways are saved
	 * @param wayRoleMap map, where roles of ways are saved
	 **/
	template<typename Target>
	inline void parseProperties(eaglexml::xml_node<>* firstProp,
								DataMap<CachedString, CachedString>* tagMap,
								std::vector<NodeId>* nodeRefIds,
								DataMap<NodeId, CachedString>* nodeRoleMap,
								std::vector<WayId>* wayRefIds,
								DataMap<WayId, CachedString>* wayRoleMap)
	{
		BOOST_STATIC_ASSERT((boost::is_same<Target, Node>::value || boost::is_same<Target, Way>::value || boost::is_same<Target, Relation>::value));
		assert(tagMap);

		// Go through all properties and parse them
		eaglexml::xml_node<>* prop = firstProp;
		for(;prop; prop = prop->next_sibling())
		{
			const char* propName = firstProp->name();

			if(std::strcmp(propName, "tag") == 0)
			{
				// Parse tag
				CachedString key;
				CachedString value;
				extractAttributeFromNode("k", prop, &key);
				extractAttributeFromNode("v", prop, &value);

				(*tagMap)[key] = value;

			}else if(boost::is_same<Target, Way>::value && std::strcmp(propName, "nd") == 0)
			{
				assert(nodeRefIds);

				// Parse a node reference
				OsmIdType osmId;
				extractAttributeFromNode("ref", prop, &osmId);
				nodeRefIds->push_back(resolveOsmId(osmId, nodeIdMapping));
			}else if(boost::is_same<Target, Relation>::value && std::strcmp(propName, "member") == 0)
			{
				// Parse a relation member
				assert(nodeRefIds);
				assert(nodeRoleMap);
				assert(wayRefIds);
				assert(wayRoleMap);


				eaglexml::xml_attribute<>* attr = prop->first_attribute("type");

				if(!attr || attr->value_size() == 0)
					BOOST_THROW_EXCEPTION(excp::InputFormatException() << excp::InfoXmlEntityName("relation::member::type"));

				/// Targets to be set
				OsmIdType osmRefId;
				extractAttributeFromNode("ref", prop, &osmRefId);

				// Evaluate type attribute
				const char* type = attr->value();
				if(std::strcmp(type, "node") == 0)
				{
					NodeId nodeId = resolveOsmId(osmRefId, nodeIdMapping);
					nodeRefIds->push_back(nodeId);
					extractAttributeFromNode("role", prop, &((*nodeRoleMap)[nodeId]));
				}else if(std::strcmp(type, "way") == 0)
				{
					WayId wayId = resolveOsmId(osmRefId, wayIdMapping);
					wayRefIds->push_back(wayId);
					extractAttributeFromNode("role", prop, &((*wayRoleMap)[wayId]));
				}else if(std::strcmp(type, "relation") == 0) {
					
					if(!outputIgnoreRelation)
					{
						log.warnStream() << "This software does not support relation member in relations!";
						log.warnStream() << "Reference is ignored!";
						outputIgnoreRelation = true;
					}
					continue;
				}else{

					BOOST_THROW_EXCEPTION(excp::InputFormatException() << excp::InfoXmlEntityName("relation::member::type::value"));
				}

			}
		}
	}

	/**
	 * @brief Extracts the value of a specified xml-attribute from a given node.
	 *
	 * Returns it in a wanted type. Throws if the attribute is missing or has a bad format. 
	 *
	 * @param attrname the name of the attribute
	 * @param node the node containing the attribute
	 * @param dest destination to copy the extracted value
	 **/
	template<typename T>
	void extractAttributeFromNode(const string& attrname, eaglexml::xml_node<>* node, T* dest)
	{
		assert(node);
		assert(dest);

		eaglexml::xml_attribute<>* attr = node->first_attribute(attrname.c_str(), attrname.size());

		if(!attr)
			BOOST_THROW_EXCEPTION(excp::InputFormatException() << excp::InfoXmlEntityName(attrname));

		const char* value = attr->value();
		try {
			*dest = boost::lexical_cast<T>(value);
		}catch(boost::bad_lexical_cast& e)
		{
			(void)e;
			BOOST_THROW_EXCEPTION(excp::InputFormatException() << excp::InfoXmlEntityName(attrname) << excp::InfoBadSourceValue(value));
		}
	}

	/**
	 * @brief Resolves a osm id into an internal id.
	 *
	 * Can only take care of objects already parsed.
	 *
	 * @param osmId to be resolved
	 * @param table containing the id mapping
	 * @return the resolved internal id
	 * @throws if the id can not be resolved.
	 **/
	template<typename IdType>
	IdType resolveOsmId(OsmIdType osmId, const boost::unordered_map<OsmIdType, IdType>& table)
	{
		auto it = table.find(osmId);

		if(it == table.end())
			BOOST_THROW_EXCEPTION(excp::BadOsmIdException() << excp::InfoUnresolvableId(osmId));

		return it->second;
	}


	virtual void on_fetch( unsigned int chars_left, unsigned int need, node_type* active_node ) {}
	virtual void on_buffer_resize() {}
	virtual void on_segment_read() {}

	virtual void on_read_begin( unsigned int segments )
	{
		int before = int(100 * (double)alreadyRead / (double)fileSize);
		alreadyRead += segments * segmentSize;
		int after  = int(100 * (double)alreadyRead / (double)fileSize);

		if(after != before)
			log.infoStream() << "Loading [" << std::min(after, 99) << "%]";
	}



private:

	//! Specifies weather the parser should ignore unknown entities.
	//! If this is false and an unknown entity appears an exception will be thrown
	bool ignoreUnknownEntities;
	
	//! Mapping from osm ids to internal ids for nodes
	boost::unordered_map<OsmIdType, NodeId>	nodeIdMapping;

	//! Mapping from osm ids to internal ids for ways
	boost::unordered_map<OsmIdType, WayId>	wayIdMapping;

	//! List to be filled with nodes
	shared_ptr< std::vector<Node> > nodes;

	//! List to be filled with ways
	shared_ptr< std::vector<Way> > ways;

	//! List to be filled with relations
	shared_ptr< std::vector<Relation> > relations;

	//! Size of the xml file in bytes
	std::uintmax_t	fileSize;

	//! Bytes already read from the xml file
	std::uintmax_t	alreadyRead;

	//! Number of bytes read by one read operation
	unsigned int	segmentSize;

	//! Log for the importer
	log4cpp::Category& log;

	//! Booleans for some output, which should only appear once
	bool outputIgnoreRelation, outputIgnoreBounds;
};









/**
 * @brief Initializes the importer
 *
 * @param config Configuration used by the importer
 **/
Importer::Importer(const shared_ptr<Configuration>& config)
	: config(config)
{
}

/**
 * @brief Parses an osm xml file specified in configuration containing osm data                                                                  
 *
 * @return created data containing the osm data
 **/
shared_ptr<Geodata> Importer::importXML()
{
	log4cpp::Category& log = log4cpp::Category::getRoot();
	OsmXmlParser parser(!config->get<bool>(opt::importer::check_xml_entities));
	shared_ptr<Geodata>	geodata = boost::make_shared<Geodata>();

	path xml_file = config->get<string>(opt::importer::path_to_osmdata);
	log.infoStream() << "Start parsing...";
	parser.parse(xml_file);

	log.infoStream() << "Insert into geodata...";
	geodata->insertNodes(parser.getParsedNodes());
	geodata->insertWays(parser.getParsedWays());
	geodata->insertRelations(parser.getParsedRelations());

	return geodata;
}
