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



#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "server/eval/eval.hpp"
#include "server/eval/stnode.hpp"

#include "server/parser/mapcss_parser.hpp"
#include "server/parser/mapcss_grammar.hpp"
#include "server/parser/parse_info_parser.hpp"

#include "server/stylesheet.hpp"
#include "server/style.hpp"



template<typename Type>
class SpecificAttributeCreator
	: public AttributeCreator
{
public:
	typedef shared_ptr<eval::Eval<Type> > StyleTemplate::*attr_member;

	SpecificAttributeCreator(attr_member member)
		: member(member)
	{
		assert(member);
	}

	void addAttribute(const shared_ptr<StyleTemplate>& styleTemplate, const string& specifier, const shared_ptr<ParserLogger>& logger, const ParseInfo& info)
	{
		assert(styleTemplate);
		(styleTemplate.get()->*member) = boost::make_shared< eval::Eval<Type> >(specifier, logger, info);
	}

private:
	attr_member member;
};

template<typename Type>
shared_ptr< SpecificAttributeCreator<Type> > CreateAttribute(shared_ptr<eval::Eval<Type> > StyleTemplate::*attr)
{
	return boost::make_shared< SpecificAttributeCreator<Type> >(attr);
}

AttributeTypes::AttributeTypes()
{
	this->add
		("fill-color", CreateAttribute(&StyleTemplate::fill_color))
		("image", CreateAttribute(&StyleTemplate::image))
		("fill-image", CreateAttribute(&StyleTemplate::fill_image))
		("z-index", CreateAttribute(&StyleTemplate::z_index))
		("width", CreateAttribute(&StyleTemplate::width))
		("color", CreateAttribute(&StyleTemplate::color))
		("casing-width", CreateAttribute(&StyleTemplate::casing_width))
		("casing-color", CreateAttribute(&StyleTemplate::casing_color))
		("font-size", CreateAttribute(&StyleTemplate::font_size))
		("font-family", CreateAttribute(&StyleTemplate::font_family))
		("font-weight", CreateAttribute(&StyleTemplate::font_weight))
		("font-style", CreateAttribute(&StyleTemplate::font_style))
		("text-color", CreateAttribute(&StyleTemplate::text_color))
		("text-offset", CreateAttribute(&StyleTemplate::text_offset))
		("text-position", CreateAttribute(&StyleTemplate::text_position))
		("text", CreateAttribute(&StyleTemplate::text))

		("text-halo-color", CreateAttribute(&StyleTemplate::text_halo_color))
		("text-halo-radius", CreateAttribute(&StyleTemplate::text_halo_radius))

		("icon-image", CreateAttribute(&StyleTemplate::icon_image))
		("icon-width", CreateAttribute(&StyleTemplate::icon_width))
		("icon-height", CreateAttribute(&StyleTemplate::icon_height))
		("icon-opacity", CreateAttribute(&StyleTemplate::icon_opacity))

		("shield-color", CreateAttribute(&StyleTemplate::shield_color))
		("shield-opacity", CreateAttribute(&StyleTemplate::shield_opacity))
		("shield-frame-color", CreateAttribute(&StyleTemplate::shield_frame_color))
		("shield-frame-width", CreateAttribute(&StyleTemplate::shield_frame_width))
		("shield-casing-color", CreateAttribute(&StyleTemplate::shield_casing_color))
		("shield-casing-width", CreateAttribute(&StyleTemplate::shield_casing_width))
		("shield-text", CreateAttribute(&StyleTemplate::shield_text))
		("shield-image", CreateAttribute(&StyleTemplate::shield_image))
		("shield-shape", CreateAttribute(&StyleTemplate::shield_shape))

		("linecap", CreateAttribute(&StyleTemplate::linecap))
		("linejoin", CreateAttribute(&StyleTemplate::linejoin))
		("casing-linecap", CreateAttribute(&StyleTemplate::casing_linecap))
		("casing-linejoin", CreateAttribute(&StyleTemplate::casing_linejoin))

		("dashes", CreateAttribute(&StyleTemplate::dashes))
		("casing-dashes", CreateAttribute(&StyleTemplate::casing_dashes))
		;
}








/**
 *	Appends a vector to another
 *
 *	\param target where another vector should be appended to
 *	\param append The vector which should be appended to the target
 */
template<typename T>
void AppendVector(std::vector<T>& target, const std::vector<T>& append)
{
	target.insert(target.end(), append.begin(), append.end());
}

/**
 *	Creates all rules for the mapcss grammar
 *
 *	\param The parser using this grammar
 */
MapCSSGrammar::MapCSSGrammar(MapCssParser& parser)
	: MapCSSGrammar::base_type(rule_stylesheet, "MapCSS-MainGrammar")
{
	using namespace qi::labels;
	using qi::uint_;
	auto& _1 = qi::_1;

	rule_tag %= +qi::char_("a-zA-Z_0-9:-");
	rule_value %= +qi::char_("a-zA-Z_0-9./-") | ('\"' >> +(qi::char_ - '\"')  >> '\"') | ('\'' >> +(qi::char_ - '\'') >> '\'');


	/******************************* Attributes *************************************/

	attribute_name = +qi::char_("a-zA-Z_0-9-");
	rule_specifier = ('\"' > +(qi::char_ - '\"') > '\"' ) | +(qi::char_ - ';' - '}');
	rule_styleset = qi::eps[
								_val = phx::construct<StylePtr>(phx::new_<StyleTemplate>())
							]
							>> -(
									(
										attributeType_[_a = _1]
										>	':' > pip::pinfo[_b = _1]
										>	rule_specifier[phx::bind(&MapCssParser::addAttributeToTemplate, &parser, _val, _a, _1, _b)]
									) | (
										attribute_name[phx::bind(&MapCssParser::warnUnsupportedAttribute, &parser, _1)]
										>   ':' > rule_specifier
									)
							) % ';'
							>> -qi::lit(';');

	/******************************* Selectors *********************************/

	//rule_condition = (rule_tag > BinaryTypes > rule_string) | (UnaryTypes > rule_tag) | rule_tag;

	rule_zoom = "|z" > qi::eps[_a = ALAC_ZOOM_BOTTOM, _b = ALAC_ZOOM_TOP] 
			>	((-uint_[_a = _1] >> '-' >> -uint_[_b = _1])
				| (uint_[_a = _b = _1]))
			>> qi::eps[_val = phx::construct<Zoom>(_a,_b)];


	rule_binary_condition %= rule_tag >> binaryOperator_ >> rule_value;// >> binaryOperator_ >> chs::string;
	rule_unary_condition %= -unaryOperator_ >> rule_tag;

	//rule_class = (lit(':') | lit('.')) > +chs::alpha;
	rule_subselector =	selectorObject_[phx::at_c<0>(_val) = _1]
									>> -rule_zoom[phx::at_c<1>(_val) = _1]
									>> *(
											'['
											> (rule_binary_condition | rule_unary_condition)
											[
												phx::push_back(phx::at_c<2>(_val), _1)
											]
											> ']'
										)
										//>> -( (lit('!') > rule_class) | rule_class)
										;

	rule_selector = (
						+rule_subselector[phx::push_back(_a, _1)]
					)
					[_val = phx::bind(&MapCssParser::createSelectorChain, &parser, _a)];


	/******************************* Stylesheet *********************************/
	rule_cssrule = (rule_selector[phx::push_back(_val, _1)] % ',')
						> '{'
						> rule_styleset[phx::bind(&MapCssParser::applyStyleToRules, &parser, _val, _1)]
						> '}';

	rule_stylesheet =	(
							*(
								rule_cssrule
									[
										phx::bind(&AppendVector<RulePtr>, _a, _1)
									]
								| (
										qi::lit("canvas") 
										> '{'
										> -rule_styleset
											[
												_b = _1
											]
										> '}'
									)
							)
						)
						[
							qi::_val = phx::construct<StylesheetPtr>(phx::new_<Stylesheet>(phx::ref(parser.geodata), _a, _b))
						];
}
