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



#include "utils/colorTable.hpp"







//! Creates the color table
ColorTable::ColorTable()
{
	table["maroon"] = uint32_t(0xFF800000);
	table["darkred"] = uint32_t(0xFF8B0000);
	table["red"] = uint32_t(0xFFFF0000);
	table["lightpink"] = uint32_t(0xFFFFB6C1);
	table["crimson"] = uint32_t(0xFFDC143C);

	table["palevioletred"] = uint32_t(0xFFDB7093);
	table["hotpink"] = uint32_t(0xFFFF69B4);
	table["deeppink"] = uint32_t(0xFFFF1493);
	table["mediumvioletred"] = uint32_t(0xFFC71585);
	table["purple"] = uint32_t(0xFF800080);

	table["darkmagenta"] = uint32_t(0xFF8B008B);
	table["orchid"] = uint32_t(0xFFDA70D6);
	table["thistle"] = uint32_t(0xFFD8BFD8);
	table["plum"] = uint32_t(0xFFDDA0DD);
	table["violet"] = uint32_t(0xFFEE82EE);

	table["fuchsia"] = uint32_t(0xFFFF00FF);
	table["magenta"] = uint32_t(0xFFFF00FF);
	table["mediumorchid"] = uint32_t(0xFFBA55D3);
	table["darkviolet"] = uint32_t(0xFF9400D3);
	table["darkorchid"] = uint32_t(0xFF9932CC);

	table["blueviolet"] = uint32_t(0xFF8A2BE2);
	table["indigo"] = uint32_t(0xFF4B0082);
	table["mediumpurple"] = uint32_t(0xFF9370DB);
	table["slateblue"] = uint32_t(0xFF6A5ACD);
	table["mediumslateblue"] = uint32_t(0xFF7B68EE);

	table["darkblue"] = uint32_t(0xFF00008B);
	table["mediumblue"] = uint32_t(0xFF0000CD);
	table["blue"] = uint32_t(0xFF0000FF);
	table["navy"] = uint32_t(0xFF000080);
	table["midnightblue"] = uint32_t(0xFF191970);

	table["darkslateblue"] = uint32_t(0xFF483D8B);
	table["royalblue"] = uint32_t(0xFF4169E1);
	table["cornflowerblue"] = uint32_t(0xFF6495ED);
	table["lightsteelblue"] = uint32_t(0xFFB0C4DE);
	table["aliceblue"] = uint32_t(0xFFF0F8FF);

	table["ghostwhite"] = uint32_t(0xFFF8F8FF);
	table["lavender"] = uint32_t(0xFFE6E6FA);
	table["dodgerblue"] = uint32_t(0xFF1E90FF);
	table["steelblue"] = uint32_t(0xFF4682B4);
	table["deepskyblue"] = uint32_t(0xFF00BFFF);

	table["slategray"] = uint32_t(0xFF708090);
	table["lightslategray"] = uint32_t(0xFF778899);
	table["lightskyblue"] = uint32_t(0xFF87CEFA);
	table["skyblue"] = uint32_t(0xFF87CEEB);
	table["lightblue"] = uint32_t(0xFFADD8E6);

	table["teal"] = uint32_t(0xFF008080);
	table["darkcyan"] = uint32_t(0xFF008B8B);
	table["darkturquoise"] = uint32_t(0xFF00CED1);
	table["aqua"] = uint32_t(0xFF00FFFF);
	table["mediumturquoise"] = uint32_t(0xFF48D1CC);

	table["cadetblue"] = uint32_t(0xFF5F9EA0);
	table["paleturquoise"] = uint32_t(0xFFAFEEEE);
	table["lightcyan"] = uint32_t(0xFFE0FFFF);
	table["azure"] = uint32_t(0xFFF0FFFF);
	table["lightseagreen"] = uint32_t(0xFF20B2AA);

	table["turquoise"] = uint32_t(0xFF40E0D0);
	table["powderblue"] = uint32_t(0xFFB0E0E6);
	table["darkslategray"] = uint32_t(0xFF2F4F4F);
	table["aquamarine"] = uint32_t(0xFF7FFFD4);
	table["mediumspringgreen"] = uint32_t(0xFF00FA9A);

	table["mediumaquamarine"] = uint32_t(0xFF66CDAA);
	table["springgreen"] = uint32_t(0xFF00FF7F);
	table["mediumseagreen"] = uint32_t(0xFF3CB371);
	table["seagreen"] = uint32_t(0xFF2E8B57);
	table["limegreen"] = uint32_t(0xFF32CD32);

	table["darkgreen"] = uint32_t(0xFF006400);
	table["green"] = uint32_t(0xFF008000);
	table["lime"] = uint32_t(0xFF00FF00);
	table["forestgreen"] = uint32_t(0xFF228B22);
	table["darkseagreen"] = uint32_t(0xFF8FBC8F);

	table["lightgreen"] = uint32_t(0xFF90EE90);
	table["palegreen"] = uint32_t(0xFF98FB98);
	table["mintcream"] = uint32_t(0xFFF5FFFA);
	table["honeydew"] = uint32_t(0xFFF0FFF0);
	table["chartreuse"] = uint32_t(0xFF7FFF00);

	table["lawngreen"] = uint32_t(0xFF7CFC00);
	table["olivedrab"] = uint32_t(0xFF6B8E23);
	table["darkolivegreen"] = uint32_t(0xFF556B2F);
	table["yellowgreen"] = uint32_t(0xFF9ACD32);
	table["greenyellow"] = uint32_t(0xFFADFF2F);

	table["beige"] = uint32_t(0xFFF5F5DC);
	table["linen"] = uint32_t(0xFFFAF0E6);
	table["lightgoldenrodyellow"] = uint32_t(0xFFFAFAD2);
	table["olive"] = uint32_t(0xFF808000);
	table["yellow"] = uint32_t(0xFFFFFF00);

	table["lightyellow"] = uint32_t(0xFFFFFFE0);
	table["ivory"] = uint32_t(0xFFFFFFF0);
	table["darkkhaki"] = uint32_t(0xFFBDB76B);
	table["khaki"] = uint32_t(0xFFF0E68C);
	table["palegoldenrod"] = uint32_t(0xFFEEE8AA);

	table["wheat"] = uint32_t(0xFFF5DEB3);
	table["gold"] = uint32_t(0xFFFFD700);
	table["lemonchiffon"] = uint32_t(0xFFFFFACD);
	table["papayawhip"] = uint32_t(0xFFFFEFD5);
	table["darkgoldenrod"] = uint32_t(0xFFB8860B);

	table["goldenrod"] = uint32_t(0xFFDAA520);
	table["antiquewhite"] = uint32_t(0xFFFAEBD7);
	table["cornsilk"] = uint32_t(0xFFFFF8DC);
	table["oldlace"] = uint32_t(0xFFFDF5E6);
	table["moccasin"] = uint32_t(0xFFFFE4B5);

	table["navajowhite"] = uint32_t(0xFFFFDEAD);
	table["orange"] = uint32_t(0xFFFFA500);
	table["bisque"] = uint32_t(0xFFFFE4C4);
	table["tan"] = uint32_t(0xFFD2B48C);
	table["darkorange"] = uint32_t(0xFFFF8C00);

	table["burlywood"] = uint32_t(0xFFDEB887);
	table["saddlebrown"] = uint32_t(0xFF8B4513);
	table["sandybrown"] = uint32_t(0xFFF4A460);
	table["blanchedalmond"] = uint32_t(0xFFFFEBCD);
	table["lavenderblush"] = uint32_t(0xFFFFF0F5);

	table["seashell"] = uint32_t(0xFFFFF5EE);
	table["floralwhite"] = uint32_t(0xFFFFFAF0);
	table["snow"] = uint32_t(0xFFFFFAFA);
	table["peru"] = uint32_t(0xFFCD853F);
	table["peachpuff"] = uint32_t(0xFFFFDAB9);

	table["chocolate"] = uint32_t(0xFFD2691E);
	table["sienna"] = uint32_t(0xFFA0522D);
	table["lightsalmon"] = uint32_t(0xFFFFA07A);
	table["coral"] = uint32_t(0xFFFF7F50);
	table["darksalmon"] = uint32_t(0xFFE9967A);

	table["mistyrose"] = uint32_t(0xFFFFE4E1);
	table["orangered"] = uint32_t(0xFFFF4500);
	table["salmon"] = uint32_t(0xFFFA8072);
	table["tomato"] = uint32_t(0xFFFF6347);
	table["rosybrown"] = uint32_t(0xFFBC8F8F);

	table["pink"] = uint32_t(0xFFFFC0CB);
	table["indianred"] = uint32_t(0xFFCD5C5C);
	table["lightcoral"] = uint32_t(0xFFF08080);
	table["brown"] = uint32_t(0xFFA52A2A);
	table["firebrick"] = uint32_t(0xFFB22222);

	table["black"] = uint32_t(0xFF000000);
	table["dimgray"] = uint32_t(0xFF696969);
	table["gray"] = uint32_t(0xFF808080);
	table["darkgray"] = uint32_t(0xFFA9A9A9);
	table["silver"] = uint32_t(0xFFC0C0C0);

	table["lightgrey"] = uint32_t(0xFFD3D3D3);
	table["gainsboro"] = uint32_t(0xFFDCDCDC);
	table["whitesmoke"] = uint32_t(0xFFF5F5F5);
	table["white"] = uint32_t(0xFFFFFFFF);
}

/**
 * @brief Returns a instance of a color table.
 * 
 * If no color table exists a new one will be created.
 *
 * \return the one and only color table
 **/
const ColorTable& ColorTable::Inst()
{
	static ColorTable inst;
	return inst;
}


/** Operator to access the color table.
 *
 * \param name of the color which should be resolved.
 * \param 
 */
void ColorTable::resolve(const std::string& name, Color* target) const
{
	assert(target);
	auto it = table.find(name);

	if(it != table.end())
	{
		*target = it->second;
	}
}

/**
 * @brief Returns the hole table
 *
 * \return Table with color mappings
 **/
const boost::unordered_map<std::string, Color>& ColorTable::getTable() const
{
	return table;
}