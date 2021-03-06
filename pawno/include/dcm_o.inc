/* Copyright (c) 2018 Anton Styazhkin
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *	claim that you wrote the original software. If you use this software
 *	in a product, an acknowledgement in the product documentation would be
 *	appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *	misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <a_samp>

#if defined dcm_o_included
	#endinput
#endif
#define dcm_o_included

#include "YSI\y_xml"
#include "YSI\y_stringhash"

#tryinclude "sscanf2"
#tryinclude "streamer"

#if !defined MAX_MAPS
	#define MAX_MAPS 100
#endif

#if defined OnMapLoad
	forward OnMapLoad(mapid, objectid, bool:dynamic);
#endif

#if defined OnMapDestroy
	forward OnMapDestroy(mapid, objectid, bool:dynamic);
#endif

static const
	MAP_OBJECT_COUNT = 0,
	MAP_LAST_OBJECT_ID = 1;

static stock
	map_id,
	map_data[MAX_MAPS][2] = {-1, ...};

stock
	DestroyMap(mapid)
{
	if (map_data[mapid][MAP_OBJECT_COUNT] == -1)
		return 0;

	new objectid;
	
	for (new i = map_data[mapid][MAP_OBJECT_COUNT]; i > -1; i--)
	{
		objectid = map_data[mapid][MAP_LAST_OBJECT_ID] - i;
		#if !defined _streamer_included
			DestroyObject(objectid);
			#if defined OnMapDestroy
				OnMapDestroy(mapid, objectid, false);
			#endif
		#else
			DestroyDynamicObject(objectid);
			#if defined OnMapDestroy
				OnMapDestroy(mapid, objectid, true);
			#endif
		#endif
	}
	map_data[map_id][MAP_LAST_OBJECT_ID] = -1;
	return 1;
}

stock
	LoadFromMapFile(path[])
{
	new
		XML:file = XML_New();
		
	XML_AddHandler(file, "object", "DCMO_OnMapLoad");
	XML_Parse(file, path);
	return map_id++;
}

public DCMO_OnMapLoad();
public DCMO_OnMapLoad()
{
	static
		key[MAX_XML_ENTRY_NAME],
		val[MAX_XML_ENTRY_TEXT];

	new
		model,
		dimension,
		interior,
		doublesided,
		Float:pos_x,
		Float:pos_y,
		Float:pos_z,
		Float:pos_rx,
		Float:pos_ry,
		Float:pos_rz;

	#define DCMO_map_is(%0,%1) (YHash(%0, false, hash_fnv1) == _I@f<%1>)
	
	while (XML_GetKeyValue(key, val))
	{
		if (DCMO_map_is(key, rotX))
		{
			#if !defined sscanf
				pos_rx = floatstr(val);
			#else
				sscanf(val, "f", pos_rx);
			#endif
		}
		if (DCMO_map_is(key, rotY))
		{
			#if !defined sscanf
				pos_ry = floatstr(val);
			#else
				sscanf(val, "f", pos_ry);
			#endif
		}
		if (DCMO_map_is(key, rotZ))
		{
			#if !defined sscanf
				pos_rz = floatstr(val);
			#else
				sscanf(val, "f", pos_rz);
			#endif
		}
		if (DCMO_map_is(key, posX))
		{
			#if !defined sscanf
				pos_x = floatstr(val);
			#else
				sscanf(val, "f", pos_x);
			#endif
		}
		if (DCMO_map_is(key, posY))
		{
			#if !defined sscanf
				pos_y = floatstr(val);
			#else
				sscanf(val, "f", pos_y);
			#endif
		}
		if (DCMO_map_is(key, posZ))
		{
			#if !defined sscanf
				pos_z = floatstr(val);
			#else
				sscanf(val, "f", pos_z);
			#endif
		}
		if (DCMO_map_is(key, dimension))
		{
			#if !defined sscanf
				dimension = strval(val);
			#else
				sscanf(val, "i", dimension);
			#endif
		}
		if (DCMO_map_is(key, interior))
		{
			#if !defined sscanf
				interior = strval(val);
			#else
				sscanf(val, "i", interior);
			#endif
		}
		if (DCMO_map_is(key, model))
		{
			#if !defined sscanf
				model = strval(val);
			#else
				sscanf(val, "i", model);
			#endif
		}
		if (DCMO_map_is(key, doublesided))
		{
			#if !defined sscanf
				doublesided = strval(val);
			#else
				sscanf(val, "i", doublesided);
			#endif
		}
		
		static objectid;
		
		map_data[map_id][MAP_OBJECT_COUNT]++;

		#pragma unused dimension
		#pragma unused interior
		#pragma unused doublesided

		#pragma unused objectid
		
		#if !defined _streamer_included
			objectid = map_data[map_id][MAP_LAST_OBJECT_ID] = CreateObject(
				model,
				pos_x,
				pos_y,
				pos_z,
				pos_rx,
				pos_ry,
				pos_rz
			);
			#if defined OnMapLoad
				OnMapLoad(map_id, objectid, false);
			#endif
		#else
			objectid = map_data[map_id][MAP_LAST_OBJECT_ID] = CreateDynamicObject(
				model,
				pos_x,
				pos_y,
				pos_z,
				pos_rx,
				pos_ry,
				pos_rz,
				.worldid = dimension,
				.interiorid = interior
			);
			#if defined OnMapLoad
				OnMapLoad(map_id, objectid, true);
			#endif
		#endif
	}

	#undef DCMO_map_is
	return 1;
}
