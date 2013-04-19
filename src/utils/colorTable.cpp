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
	table["maroon"] = uint32(0xFF800000);
	table["darkred"] = uint32(0xFF8B0000);
	table["red"] = uint32(0xFFFF0000);
	table["lightpink"] = uint32(0xFFFFB6C1);
	table["crimson"] = uint32(0xFFDC143C);

	table["palevioletred"] = uint32(0xFFDB7093);
	table["hotpink"] = uint32(0xFFFF69B4);
	table["deeppink"] = uint32(0xFFFF1493);
	table["mediumvioletred"] = uint32(0xFFC71585);
	table["purple"] = uint32(0xFF800080);

	table["darkmagenta"] = uint32(0xFF8B008B);
	table["orchid"] = uint32(0xFFDA70D6);
	table["thistle"] = uint32(0xFFD8BFD8);
	table["plum"] = uint32(0xFFDDA0DD);
	table["violet"] = uint32(0xFFEE82EE);

	table["fuchsia"] = uint32(0xFFFF00FF);
	table["magenta"] = uint32(0xFFFF00FF);
	table["mediumorchid"] = uint32(0xFFBA55D3);
	table["darkviolet"] = uint32(0xFF9400D3);
	table["darkorchid"] = uint32(0xFF9932CC);

	table["blueviolet"] = uint32(0xFF8A2BE2);
	table["indigo"] = uint32(0xFF4B0082);
	table["mediumpurple"] = uint32(0xFF9370DB);
	table["slateblue"] = uint32(0xFF6A5ACD);
	table["mediumslateblue"] = uint32(0xFF7B68EE);

	table["darkblue"] = uint32(0xFF00008B);
	table["mediumblue"] = uint32(0xFF0000CD);
	table["blue"] = uint32(0xFF0000FF);
	table["navy"] = uint32(0xFF000080);
	table["midnightblue"] = uint32(0xFF191970);

	table["darkslateblue"] = uint32(0xFF483D8B);
	table["royalblue"] = uint32(0xFF4169E1);
	table["cornflowerblue"] = uint32(0xFF6495ED);
	table["lightsteelblue"] = uint32(0xFFB0C4DE);
	table["aliceblue"] = uint32(0xFFF0F8FF);

	table["ghostwhite"] = uint32(0xFFF8F8FF);
	table["lavender"] = uint32(0xFFE6E6FA);
	table["dodgerblue"] = uint32(0xFF1E90FF);
	table["steelblue"] = uint32(0xFF4682B4);
	table["deepskyblue"] = uint32(0xFF00BFFF);

	table["slategray"] = uint32(0xFF708090);
	table["lightslategray"] = uint32(0xFF778899);
	table["lightskyblue"] = uint32(0xFF87CEFA);
	table["skyblue"] = uint32(0xFF87CEEB);
	table["lightblue"] = uint32(0xFFADD8E6);

	table["teal"] = uint32(0xFF008080);
	table["darkcyan"] = uint32(0xFF008B8B);
	table["darkturquoise"] = uint32(0xFF00CED1);
	table["aqua"] = uint32(0xFF00FFFF);
	table["mediumturquoise"] = uint32(0xFF48D1CC);

	table["cadetblue"] = uint32(0xFF5F9EA0);
	table["paleturquoise"] = uint32(0xFFAFEEEE);
	table["lightcyan"] = uint32(0xFFE0FFFF);
	table["azure"] = uint32(0xFFF0FFFF);
	table["lightseagreen"] = uint32(0xFF20B2AA);

	table["turquoise"] = uint32(0xFF40E0D0);
	table["powderblue"] = uint32(0xFFB0E0E6);
	table["darkslategray"] = uint32(0xFF2F4F4F);
	table["aquamarine"] = uint32(0xFF7FFFD4);
	table["mediumspringgreen"] = uint32(0xFF00FA9A);

	table["mediumaquamarine"] = uint32(0xFF66CDAA);
	table["springgreen"] = uint32(0xFF00FF7F);
	table["mediumseagreen"] = uint32(0xFF3CB371);
	table["seagreen"] = uint32(0xFF2E8B57);
	table["limegreen"] = uint32(0xFF32CD32);

	table["darkgreen"] = uint32(0xFF006400);
	table["green"] = uint32(0xFF008000);
	table["lime"] = uint32(0xFF00FF00);
	table["forestgreen"] = uint32(0xFF228B22);
	table["darkseagreen"] = uint32(0xFF8FBC8F);

	table["lightgreen"] = uint32(0xFF90EE90);
	table["palegreen"] = uint32(0xFF98FB98);
	table["mintcream"] = uint32(0xFFF5FFFA);
	table["honeydew"] = uint32(0xFFF0FFF0);
	table["chartreuse"] = uint32(0xFF7FFF00);

	table["lawngreen"] = uint32(0xFF7CFC00);
	table["olivedrab"] = uint32(0xFF6B8E23);
	table["darkolivegreen"] = uint32(0xFF556B2F);
	table["yellowgreen"] = uint32(0xFF9ACD32);
	table["greenyellow"] = uint32(0xFFADFF2F);

	table["beige"] = uint32(0xFFF5F5DC);
	table["linen"] = uint32(0xFFFAF0E6);
	table["lightgoldenrodyellow"] = uint32(0xFFFAFAD2);
	table["olive"] = uint32(0xFF808000);
	table["yellow"] = uint32(0xFFFFFF00);

	table["lightyellow"] = uint32(0xFFFFFFE0);
	table["ivory"] = uint32(0xFFFFFFF0);
	table["darkkhaki"] = uint32(0xFFBDB76B);
	table["khaki"] = uint32(0xFFF0E68C);
	table["palegoldenrod"] = uint32(0xFFEEE8AA);

	table["wheat"] = uint32(0xFFF5DEB3);
	table["gold"] = uint32(0xFFFFD700);
	table["lemonchiffon"] = uint32(0xFFFFFACD);
	table["papayawhip"] = uint32(0xFFFFEFD5);
	table["darkgoldenrod"] = uint32(0xFFB8860B);

	table["goldenrod"] = uint32(0xFFDAA520);
	table["antiquewhite"] = uint32(0xFFFAEBD7);
	table["cornsilk"] = uint32(0xFFFFF8DC);
	table["oldlace"] = uint32(0xFFFDF5E6);
	table["moccasin"] = uint32(0xFFFFE4B5);

	table["navajowhite"] = uint32(0xFFFFDEAD);
	table["orange"] = uint32(0xFFFFA500);
	table["bisque"] = uint32(0xFFFFE4C4);
	table["tan"] = uint32(0xFFD2B48C);
	table["darkorange"] = uint32(0xFFFF8C00);

	table["burlywood"] = uint32(0xFFDEB887);
	table["saddlebrown"] = uint32(0xFF8B4513);
	table["sandybrown"] = uint32(0xFFF4A460);
	table["blanchedalmond"] = uint32(0xFFFFEBCD);
	table["lavenderblush"] = uint32(0xFFFFF0F5);

	table["seashell"] = uint32(0xFFFFF5EE);
	table["floralwhite"] = uint32(0xFFFFFAF0);
	table["snow"] = uint32(0xFFFFFAFA);
	table["peru"] = uint32(0xFFCD853F);
	table["peachpuff"] = uint32(0xFFFFDAB9);

	table["chocolate"] = uint32(0xFFD2691E);
	table["sienna"] = uint32(0xFFA0522D);
	table["lightsalmon"] = uint32(0xFFFFA07A);
	table["coral"] = uint32(0xFFFF7F50);
	table["darksalmon"] = uint32(0xFFE9967A);

	table["mistyrose"] = uint32(0xFFFFE4E1);
	table["orangered"] = uint32(0xFFFF4500);
	table["salmon"] = uint32(0xFFFA8072);
	table["tomato"] = uint32(0xFFFF6347);
	table["rosybrown"] = uint32(0xFFBC8F8F);

	table["pink"] = uint32(0xFFFFC0CB);
	table["indianred"] = uint32(0xFFCD5C5C);
	table["lightcoral"] = uint32(0xFFF08080);
	table["brown"] = uint32(0xFFA52A2A);
	table["firebrick"] = uint32(0xFFB22222);

	table["black"] = uint32(0xFF000000);
	table["dimgray"] = uint32(0xFF696969);
	table["gray"] = uint32(0xFF808080);
	table["darkgray"] = uint32(0xFFA9A9A9);
	table["silver"] = uint32(0xFFC0C0C0);

	table["lightgrey"] = uint32(0xFFD3D3D3);
	table["gainsboro"] = uint32(0xFFDCDCDC);
	table["whitesmoke"] = uint32(0xFFF5F5F5);
	table["white"] = uint32(0xFFFFFFFF);
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
void ColorTable::resolve(const string& name, Color* target) const
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
const boost::unordered_map<string, Color>& ColorTable::getTable() const
{
	return table;
}