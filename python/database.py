#!/usr/bin/python3
##########################################
# Routino database access from Python.
#
# Part of the Routino routing software.
##########################################
# This file Copyright 2018 Andrew M. Bishop
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##########################################

import routino.database


# Database, access all attributes

database = routino.database.LoadDatabase("../../src/test/fat", "turns")

if database is None:
    database = routino.database.LoadDatabase("../src/test/fat", "turns")

    if database is None:
        print("Failed to load database")
        exit(1)

print(database)

database_attrs = ['nnodes', 'nsegments', 'nways', 'nrelations']

for attr in database_attrs:
    print("   Attribute: " + attr + " =", getattr(database, attr))

print("")


# A single node, access all attributes and all functions

node=database.GetNode(0)

print("1st node =", node)

node_attrs = ['id', 'firstsegment', 'latitude', 'longitude', 'allow',     'flags']
node_infos = ['',   '',              'degrees', 'degrees',   '[note 1]',  '[note 2]']

for attr,info in zip(node_attrs,node_infos):
    print("   Attribute: " + attr + " =", getattr(node, attr), info)

segments = node.Segments()
print("   Function: " + "Segments()" + " = [" + ", ".join([str(segments[x]) for x in range(len(segments))]) + "]")

print("")


# A single segment, access all attributes and all functions

segment=database.GetSegment(0)

print("1st segment =", segment)

segment_attrs = ['id', 'node1', 'node2', 'next2', 'way', 'distance', 'flags']
segment_infos = ['',   '',      '',      '',      '',    'km',       '[note 3]']

for attr,info in zip(segment_attrs,segment_infos):
    print("   Attribute: " + attr + " =", getattr(segment, attr), info)

print("   Function: " + "Node1()" + " = " + str(segment.Node1()))
print("   Function: " + "Node2()" + " = " + str(segment.Node2()))
print("   Function: " + "Way()" + " = " + str(segment.Way()))

print("")


# A single way, access all attributes and all functions

way=database.GetWay(0)

print("1st way =", way)

way_attrs = ['id', 'name', 'allow',    'type',     'props',    'speed',          'weight',          'height',          'width',           'length']
way_infos = ['',   '',     '[note 1]', '[note 4]', '[note 5]', 'km/hr [note 6]', 'tonnes [note 6]', 'metres [note 6]', 'metres [note 6]', 'metres [note 6]']

for attr,info in zip(way_attrs,way_infos):
    print("   Attribute: " + attr + " =", getattr(way, attr), info)

print("")


# A single relation, access all attributes and all functions

relation=database.GetRelation(0)

print("1st relation =", relation)

relation_attrs = ['id', 'from_seg', 'via_node', 'to_seg', 'from_way', 'to_way', 'from_node', 'to_node', 'except_transport']
relation_infos = ['',   '',         '',         '',       '',         '',       '',          '',        '[note 7]']

for attr,info in zip(relation_attrs,relation_infos):
    print("   Attribute: " + attr + " =", getattr(relation, attr), info)

print("   Function: " + "FromSegment()" + " = " + str(relation.FromSegment()))
print("   Function: " + "ViaNode()" + " = " + str(relation.ViaNode()))
print("   Function: " + "ToSegment()" + " = " + str(relation.ToSegment()))

print("   Function: " + "FromWay()" + " = " + str(relation.FromWay()))
print("   Function: " + "ToWay()" + " = " + str(relation.ToWay()))

print("   Function: " + "FromNode()" + " = " + str(relation.FromNode()))
print("   Function: " + "ToNode()" + " = " + str(relation.ToNode()))

print("")


# The list of nodes as a list and an iterable (just the first 4)

nodes=database.Nodes()

print("len(database.Nodes()) = " + str(len(nodes)))

print("database.Nodes() = [" + ", ".join([str(nodes[x]) for x in range(4)]) + ", ...]")

for node in nodes:
    if node.id == 4:
        break
    print(node)

print("")


# The list of segments as a list and an iterable (just the first 4)

segments=database.Segments()

print("len(database.Segments()) = " + str(len(segments)))

print("database.Segments() = [" + ", ".join([str(segments[x]) for x in range(4)]) + ", ...]")

for segment in segments:
    if segment.id == 4:
        break
    print(segment)

print("")


# The list of ways as a list and an iterable (just the first 4)

ways=database.Ways()

print("len(database.Ways()) = " + str(len(ways)))

print("database.Ways() = [" + ", ".join([str(ways[x]) for x in range(4)]) + ", ...]")

for way in ways:
    if way.id == 4:
        break
    print(way)

print("")


# The list of relations as a list and an iterable (just the first 4)

relations=database.Relations()

print("len(database.Relations()) = " + str(len(relations)))

print("database.Relations() = [" + ", ".join([str(relations[x]) for x in range(4)]) + ", ...]")

for relation in relations:
    if relation.id == 4:
        break
    print(relation)

print("")


# Enumerated lists

transports_enum = ["Transports_None",
                   "Transports_Foot",
                   "Transports_Horse",
                   "Transports_Wheelchair",
                   "Transports_Bicycle",
                   "Transports_Moped",
                   "Transports_Motorcycle",
                   "Transports_Motorcar",
                   "Transports_Goods",
                   "Transports_HGV",
                   "Transports_PSV",
                   "Transports_ALL"]

nodeflags_enum = ["Nodeflag_Super",
                  "Nodeflag_U_Turn",
                  "Nodeflag_Mini_Roundabout",
                  "Nodeflag_Turn_Restrict",
                  "Nodeflag_Turn_Restrict2"]

segmentflags_enum = ["Segmentflag_Area",
                     "Segmentflag_Oneway_1to2",
                     "Segmentflag_Oneway_2to1",
                     "Segmentflag_Super",
                     "Segmentflag_Normal"]

properties_enum = ["Properties_None",
                   "Properties_Paved",
                   "Properties_Multilane",
                   "Properties_Bridge",
                   "Properties_Tunnel",
                   "Properties_FootRoute",
                   "Properties_BicycleRoute",
                   "Properties_ALL"]

highway_enum = ["Highway_Motorway",
                "Highway_Trunk",
                "Highway_Primary",
                "Highway_Secondary",
                "Highway_Tertiary",
                "Highway_Unclassified",
                "Highway_Residential",
                "Highway_Service",
                "Highway_Track",
                "Highway_Cycleway",
                "Highway_Path",
                "Highway_Steps",
                "Highway_Ferry",
                "Highway_Count",
                "Highway_CycleBothWays",
                "Highway_OneWay",
                "Highway_Roundabout",
                "Highway_Area"]

def print_enum(list):
    for item in list:
        print("    routino.database."+item)


print("Note 1: The Node's and Way's 'allow' parameter can be the combination of these enumerated values:")
print_enum(transports_enum)
print("")
print("Note 2: The Node's 'flags' parameter can be the combination of these enumerated values:")
print_enum(nodeflags_enum)
print("")
print("Note 3: The Segment's 'flags' parameter can be the combination of these enumerated values:")
print_enum(segmentflags_enum)
print("")
print("Note 4: The Way's 'type' parameter can be one the combination of these enumerated values:")
print_enum(highway_enum)
print("")
print("Note 5: The Way's 'props' parameter can be the combination of these enumerated values:")
print_enum(properties_enum)
print("")
print("Note 6: A value of zero for a Way's speed, weight, height, width or length means that there is no limit.")
print("")
print("Note 7: The Relation's 'except_transport' parameter can be the combination of these enumerated values:")
print_enum(transports_enum)
print("")


import gc

gc.collect()
