/***************************************
 Header file for interface between Routino database and Python.

 Part of the Routino routing software.
 ******************/ /******************
 This file Copyright 2018 Andrew M. Bishop

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************/

#ifndef DATABASE_H
#define DATABASE_H    /*+ To stop multiple inclusions. +*/

#include <vector>

extern "C" {

#include "types.h"

#include "routino.h"

}


/* Constants that are not automatically picked up from types.h */

const nodeflags_t Nodeflag_Super           = NODE_SUPER;
const nodeflags_t Nodeflag_U_Turn          = NODE_UTURN;
const nodeflags_t Nodeflag_Mini_Roundabout = NODE_MINIRNDBT;
const nodeflags_t Nodeflag_Turn_Restrict   = NODE_TURNRSTRCT;
const nodeflags_t Nodeflag_Turn_Restrict2  = NODE_TURNRSTRCT2;

const distance_t Segmentflag_Area          = SEGMENT_AREA;
const distance_t Segmentflag_Oneway_1to2   = ONEWAY_1TO2;
const distance_t Segmentflag_Oneway_2to1   = ONEWAY_2TO1;
const distance_t Segmentflag_Super         = SEGMENT_SUPER;
const distance_t Segmentflag_Normal        = SEGMENT_NORMAL;


/* Classes (much easier to use them than C for doing this with swig) */

class PythonDatabase;

class PythonNode;
class PythonSegment;
class PythonWay;
class PythonRelation;

template <class T> class PythonDatabaseIter;
template <class T> class PythonNodeIter;


/* The database as seen by Python */

PythonDatabase *LoadDatabase(const char *dirname, const char *prefix);
PythonDatabase *LoadDatabase(const char *dirname);

class PythonDatabase
{
public:
 PythonDatabase(const char *_dirname,const char *_prefix, Routino_Database* database); /*+ A constructor +*/
 ~PythonDatabase();                                       /*+ A destructor to unload the database. +*/

 PythonNode     *GetNode(index_t item);     /*+ Get a single node from the database. +*/
 PythonSegment  *GetSegment(index_t item);  /*+ Get a single segment from the database. +*/
 PythonWay      *GetWay(index_t item);      /*+ Get a single way from the database. +*/
 PythonRelation *GetRelation(index_t item); /*+ Get a single relation from the database. +*/

 PythonDatabaseIter<PythonNode>     *Nodes();     /*+ Create a node iterator to get all the nodes from the database. +*/
 PythonDatabaseIter<PythonSegment>  *Segments();  /*+ Create a segment iterator to get all the segments from the database. +*/
 PythonDatabaseIter<PythonWay>      *Ways();      /*+ Create a way iterator to get all the ways from the database. +*/
 PythonDatabaseIter<PythonRelation> *Relations(); /*+ Create a relation iterator to get all the relations from the database. +*/

 index_t nnodes;                /*+ The number of nodes in the database. +*/
 index_t nsegments;             /*+ The number of segments in the database. +*/
 index_t nways;                 /*+ The number of ways in the database. +*/
 index_t nrelations;            /*+ The number of relations in the database. +*/

 char          *__str__();      /*+ Convert the Python database to a string. +*/

 friend class PythonNode;
 friend class PythonSegment;
 friend class PythonWay;
 friend class PythonRelation;

 private:

 char             *dirname;     /*+ A copy of the database directory name. +*/
 char             *prefix;      /*+ A copy of the database prefix. +*/

 Routino_Database *database;    /*+ The database opened using the libroutino function. +*/
};


/* A node as seen by Python - copied from ../src/nodes.h and then modified */

class PythonNode
{
public:
 PythonNode(PythonDatabase* _pydatabase) { pydatabase = _pydatabase; } /*+ A constructor passed the database. +*/

 index_t      id;               /*+ The index of this node. +*/

 index_t      firstsegment;     /*+ The index of the first segment. +*/

 PythonNodeIter<PythonSegment> *Segments();

 double       latitude;         /*+ The node latitude in degrees. +*/
 double       longitude;        /*+ The node longitude in degrees. +*/

 transports_t allow;            /*+ The types of transport that are allowed through the node. +*/
 nodeflags_t  flags;            /*+ Flags containing extra information (e.g. super-node, turn restriction). +*/

 char         *__str__();       /*+ Convert the Python node to a string. +*/

 private:

 friend class PythonNodeIter<PythonSegment>;

 PythonDatabase *pydatabase;    /*+ A pointer to the database that this node came from. +*/

 std::vector<index_t> segments; /*+ The list of segments for this node, only filled in after calling Segments(). +*/

 PythonSegment *get_segment(index_t item); /*+ Get a single segment from the node. +*/
 void           fill_segments(); /*+ Fill in the list of segments. +*/
};


/* A segment as seen by Python - copied from ../src/segments.h and then modified */

class PythonSegment
{
public:
 PythonSegment(PythonDatabase* _pydatabase) { pydatabase = _pydatabase; } /*+ A constructor passed the database. +*/

 index_t    id;                 /*+ The index of this segment. +*/

 index_t    node1;              /*+ The index of the starting node. +*/
 index_t    node2;              /*+ The index of the finishing node. +*/

 PythonNode *Node1() { return pydatabase->GetNode(node1); }
 PythonNode *Node2() { return pydatabase->GetNode(node2); }

 index_t    next2;              /*+ The index of the next segment sharing node2. +*/

 index_t    way;                /*+ The index of the way associated with the segment. +*/

 PythonWay  *Way() { return pydatabase->GetWay(way); }

 double     distance;           /*+ The distance between the nodes. +*/

 distance_t flags;              /*+ The flags associated with the segment. +*/

 char       *__str__();         /*+ Convert the Python segment to a string. +*/

 private:

 PythonDatabase *pydatabase;    /*+ A pointer to the database that this segment came from. +*/
};


/* A way as seen by Python - copied from ../src/ways.h and then modified */

class PythonWay
{
public:
 PythonWay(PythonDatabase* _pydatabase) { pydatabase = _pydatabase; } /*+ A constructor passed the database. +*/

 index_t      id;               /*+ The index of this way. +*/

 char         *name;            /*+ The offset of the name of the way in the names array. +*/

 transports_t allow;            /*+ The type of traffic allowed on the way. +*/

 highway_t    type;             /*+ The highway type of the way. +*/

 properties_t props;            /*+ The properties of the way. +*/

 double       speed;            /*+ The defined maximum speed limit of the way. +*/

 double       weight;           /*+ The defined maximum weight of traffic on the way. +*/
 double       height;           /*+ The defined maximum height of traffic on the way. +*/
 double       width;            /*+ The defined maximum width of traffic on the way. +*/
 double       length;           /*+ The defined maximum length of traffic on the way. +*/

 char         *__str__();       /*+ Convert the Python way to a string. +*/

 private:

 PythonDatabase *pydatabase;    /*+ A pointer to the database that this segment came from. +*/
};


/* A relation as seen by Python - copied from ../src/relations.h and then modified */

class PythonRelation
{
public:
 PythonRelation(PythonDatabase* _pydatabase) { pydatabase = _pydatabase; } /*+ A constructor passed the database. +*/

 index_t       id;              /*+ The index of this relation. +*/

 index_t       from_seg;        /*+ The segment that the path comes from. +*/
 index_t       via_node;        /*+ The node that the path goes via. +*/
 index_t       to_seg;          /*+ The segment that the path goes to. +*/

 PythonSegment *FromSegment() { return pydatabase->GetSegment(from_seg); }
 PythonNode    *ViaNode()     { return pydatabase->GetNode(via_node); }
 PythonSegment *ToSegment()   { return pydatabase->GetSegment(to_seg); }

 index_t       from_way;        /*+ The way that the path comes from. +*/
 index_t       to_way;          /*+ The way that the path goes to. +*/

 PythonWay     *FromWay()     { return pydatabase->GetWay(from_way); }
 PythonWay     *ToWay()       { return pydatabase->GetWay(to_way); }

 index_t       from_node;       /*+ The node that the path comes from. +*/
 index_t       to_node;         /*+ The node that the path goes to. +*/

 PythonNode    *FromNode()    { return pydatabase->GetNode(from_node); }
 PythonNode    *ToNode()      { return pydatabase->GetNode(to_node); }

 transports_t  except_transport; /*+ The types of transports that that this relation does not apply to. +*/

 char          *__str__();      /*+ Convert the Python relation to a string. +*/

 private:

 PythonDatabase *pydatabase;    /*+ A pointer to the database that this segment came from. +*/
};


/* A generic node/segment/way/relation iterator */

template <class T> class PythonDatabaseIter
{
 public:

 PythonDatabaseIter(PythonDatabase* _pydatabase, index_t _number) { pydatabase = _pydatabase; number = _number; } /*+ A constructor passed the database. +*/

 index_t __len__() { return number; } /*+ When used as a list return the length of it. +*/
 T      *__getitem__(index_t index);  /*+ When used as a list get a particular item from it. +*/

 PythonDatabaseIter *__iter__() { return this; } /*+ When used as an iterator return itself. +*/
 T                  *__next__() { if( next < number ) return __getitem__(next++); else return NULL; } /*+ When used as an iterator return the next item. +*/

 private:

 index_t next=0;                /*+ The next node/segment/way/relation to be returned. +*/
 index_t number;                /*+ The number of nodes/segments/ways/relations in total. +*/

 PythonDatabase *pydatabase;    /*+ A pointer to the database that this node/segment/way/relation came from. +*/
};


/* A segment iterator for nodes */

template <class T> class PythonNodeIter
{
 public:

 PythonNodeIter(PythonNode *_pynode, index_t _number) { pynode = _pynode; number = _number; } /*+ A constructor passed the node. +*/

 index_t __len__() { return number; } /*+ When used as a list return the length of it. +*/
 T      *__getitem__(index_t index);  /*+ When used as a list get a particular item from it. +*/

 PythonNodeIter *__iter__() { return this; } /*+ When used as an iterator return itself. +*/
 T              *__next__() { if( next < number ) return __getitem__(next++); else return NULL; } /*+ When used as an iterator return the next item. +*/

 private:

 index_t next=0;                /*+ The next segment to be returned. +*/
 index_t number;                /*+ The number of segments in total. +*/

 PythonNode *pynode;            /*+ A pointer to the node that these segments come from. +*/
};

#endif /* DATABASE_H */
