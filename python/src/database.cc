/***************************************
 Routino database access from Python.

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


#include <vector>

extern "C" {

#include "types.h"

#include "nodes.h"
#include "segments.h"
#include "ways.h"
#include "relations.h"

#include "routino.h"

}

#include "database.hh"


/* Copied from ../src/routino.c */

struct _Routino_Database
{
 Nodes      *nodes;
 Segments   *segments;
 Ways       *ways;
 Relations  *relations;
};


/*++++++++++++++++++++++++++++++++++++++
  Create the PythonDatabase object by loading the database and filling in some useful information.

  PythonDatabase *LoadDatabase Return a pointer to the Python view of the database.

  const char *dirname The name of the directory.

  const char *prefix The filename prefix (or NULL).
  ++++++++++++++++++++++++++++++++++++++*/

PythonDatabase *LoadDatabase(const char *dirname,const char *prefix)
{
 Routino_Database *database = Routino_LoadDatabase(dirname, prefix);

 if(!database)
    return NULL;
 else
    return new PythonDatabase(dirname, prefix, database);
}


/*++++++++++++++++++++++++++++++++++++++
  Create the PythonDatabase object by loading the database and filling in some useful information.

  PythonDatabase LoadDatabase Return a pointer to the Python view of the database.

  const char *dirname The name of the directory.
  ++++++++++++++++++++++++++++++++++++++*/

PythonDatabase *LoadDatabase(const char *dirname)
{
 return LoadDatabase(dirname,NULL);
}


/*++++++++++++++++++++++++++++++++++++++
  Create the PythonDatabase by passing it a loaded database.

  const char *_dirname The name of the directory.

  const char *_prefix The filename prefix (or NULL).

  Routino_Database *_database The opened database.
  ++++++++++++++++++++++++++++++++++++++*/

PythonDatabase::PythonDatabase(const char *_dirname,const char *_prefix, Routino_Database *_database)
{
 database = _database;

 /* Copy the database path information */

 dirname = new char[strlen(_dirname)+1];
 strcpy(dirname,_dirname);

 prefix = new char[strlen(_prefix)+1];
 strcpy(prefix,_prefix);

 /* Fill in the extra information */

 nnodes     = database->segments->file.number;
 nsegments  = database->nodes->file.number;
 nways      = database->ways->file.number;
 nrelations = database->relations->file.trnumber;
}


/*++++++++++++++++++++++++++++++++++++++
  Destroy the PythonDatabase by unloading the database.
  ++++++++++++++++++++++++++++++++++++++*/

PythonDatabase::~PythonDatabase()
{
 Routino_UnloadDatabase(database);

 delete[] dirname;
 delete[] prefix;
}


/*++++++++++++++++++++++++++++++++++++++
  Return a pointer to a modified Node data structure for use by Python.

  PythonNode *GetNode Returns a pointer to the Python view of the node.

  index_t item The index number of the Node.
  ++++++++++++++++++++++++++++++++++++++*/

PythonNode *PythonDatabase::GetNode(index_t item)
{
 PythonNode *pynode=new PythonNode(this);

 Node *nodep=LookupNode(database->nodes,item,1);
 double latitude,longitude;

 GetLatLong(database->nodes,item,nodep,&latitude,&longitude);

 pynode->id = item;

 pynode->firstsegment = nodep->firstseg;

 pynode->latitude = radians_to_degrees(latitude);
 pynode->longitude = radians_to_degrees(longitude);

 pynode->allow = nodep->allow;
 pynode->flags = nodep->flags;

 return pynode;
}


/*++++++++++++++++++++++++++++++++++++++
  Return a pointer to a modified Segment data structure for use by Python.

  PythonSegment *GetSegment Returs a pointer to the Python view of the segment.

  index_t item The index number of the Segment.
  ++++++++++++++++++++++++++++++++++++++*/

PythonSegment *PythonDatabase::GetSegment(index_t item)
{
 PythonSegment *pysegment=new PythonSegment(this);

 Segment *segmentp=LookupSegment(database->segments,item,1);

 pysegment->id = item;

 pysegment->node1 = segmentp->node1;
 pysegment->node2 = segmentp->node2;

 pysegment->next2 = segmentp->next2;

 pysegment->way = segmentp->way;

 pysegment->distance = distance_to_km(DISTANCE(segmentp->distance));

 pysegment->flags = DISTFLAG(segmentp->distance);

 return pysegment;
}


/*++++++++++++++++++++++++++++++++++++++
  Return a pointer to a modified Way data structure for use by Python.

  PythonWay *GetWay Returs a pointer to the Python view of the way.

  index_t item The index number of the Way.
  ++++++++++++++++++++++++++++++++++++++*/

PythonWay *PythonDatabase::GetWay(index_t item)
{
 PythonWay *pyway=new PythonWay(this);

 Way *wayp=LookupWay(database->ways,item,1);
 char *name=WayName(database->ways,wayp);

 pyway->id = item;

 pyway->name = name;

 pyway->allow = wayp->allow;

 pyway->type = wayp->type;

 pyway->props = wayp->props;

 pyway->speed = speed_to_kph(wayp->speed);

 pyway->weight = weight_to_tonnes(wayp->weight);
 pyway->height = height_to_metres(wayp->height);
 pyway->width  = width_to_metres(wayp->width);
 pyway->length = length_to_metres(wayp->length);

 return pyway;
}


/*++++++++++++++++++++++++++++++++++++++
  Return a pointer to a modified Relation data structure for use by Python.

  PythonRelation *GetRelation Returs a pointer to the Python view of the relation.

  index_t item The index number of the Relation.
  ++++++++++++++++++++++++++++++++++++++*/

PythonRelation *PythonDatabase::GetRelation(index_t item)
{
 PythonRelation *pyrelation=new PythonRelation(this);

 TurnRelation *relationp=LookupTurnRelation(database->relations,item,1);

 pyrelation->id = item;

 pyrelation->from_seg = relationp->from;
 pyrelation->via_node = relationp->via;
 pyrelation->to_seg   = relationp->to;

 Node *nodep=LookupNode(database->nodes,relationp->via,1);
 index_t from_way=NO_WAY,to_way=NO_WAY;
 index_t from_node=NO_NODE,to_node=NO_NODE;

 Segment *segmentp=FirstSegment(database->segments,nodep,1);

 do
   {
    index_t seg=IndexSegment(database->segments,segmentp);

    if(seg==relationp->from)
      {
       from_node=OtherNode(segmentp,relationp->via);
       from_way=segmentp->way;
      }

    if(seg==relationp->to)
      {
       to_node=OtherNode(segmentp,relationp->via);
       to_way=segmentp->way;
      }

    segmentp=NextSegment(database->segments,segmentp,relationp->via);
   }
 while(segmentp);

 pyrelation->from_way = from_way;
 pyrelation->to_way   = to_way;

 pyrelation->from_node = from_node;
 pyrelation->to_node   = to_node;

 pyrelation->except_transport = relationp->except;

 return pyrelation;
}


/*++++++++++++++++++++++++++++++++++++++
  Create an iterator so that we can iterate through all nodes in the database.

  PythonDatabaseIter<PythonNode> *PythonDatabase::Nodes Returns a pointer to a node iterator.
  ++++++++++++++++++++++++++++++++++++++*/

PythonDatabaseIter<PythonNode> *PythonDatabase::Nodes()
{
 return  new PythonDatabaseIter<PythonNode>(this,nnodes);
}


/*++++++++++++++++++++++++++++++++++++++
  Create an iterator so that we can iterate through all segments in the database.

  PythonDatabaseIter<PythonSegment> *PythonDatabase::Segments Returns a pointer to a segment iterator.
  ++++++++++++++++++++++++++++++++++++++*/

PythonDatabaseIter<PythonSegment> *PythonDatabase::Segments()
{
 return new PythonDatabaseIter<PythonSegment>(this,nsegments);
}


/*++++++++++++++++++++++++++++++++++++++
  Create an iterator so that we can iterate through all ways in the database.

  PythonDatabaseIter<PythonWay> *PythonDatabase::Ways Returns a pointer to a way iterator.
  ++++++++++++++++++++++++++++++++++++++*/

PythonDatabaseIter<PythonWay> *PythonDatabase::Ways()
{
 return new PythonDatabaseIter<PythonWay>(this,nways);
}


/*++++++++++++++++++++++++++++++++++++++
  Create an iterator so that we can iterate through all relations in the database.

  PythonDatabaseIter<PythonRelation> *PythonDatabase::Relations Returns a pointer to a relation iterator.
  ++++++++++++++++++++++++++++++++++++++*/

PythonDatabaseIter<PythonRelation> *PythonDatabase::Relations()
{
 return new PythonDatabaseIter<PythonRelation>(this,nrelations);
}


/*++++++++++++++++++++++++++++++++++++++
  Fill in the segments array so that we can access all segments on the node.
  ++++++++++++++++++++++++++++++++++++++*/

void PythonNode::fill_segments()
{
 if(segments.size()==0)
   {
    Node *nodep=LookupNode(pydatabase->database->nodes,id,1);
    Segment *segmentp=FirstSegment(pydatabase->database->segments,nodep,1);

    do
      {
       index_t seg=IndexSegment(pydatabase->database->segments,segmentp);

       segments.push_back(seg);

       segmentp=NextSegment(pydatabase->database->segments,segmentp,id);
      }
    while(segmentp);
   }
}


/*++++++++++++++++++++++++++++++++++++++
  Create an iterator so that we can iterate through all segments on the node.

  PythonNodeIter<PythonSegment> *PythonNode::Segments Returns a pointer to a segment iterator.
  ++++++++++++++++++++++++++++++++++++++*/

PythonNodeIter<PythonSegment> *PythonNode::Segments()
{
 fill_segments();

 PythonNodeIter<PythonSegment> *pyiter=new PythonNodeIter<PythonSegment>(this,segments.size());

 return pyiter;
}


/*++++++++++++++++++++++++++++++++++++++
  Get a segment from the set of segments on the node.

  PythonSegment *PythonNode::get_segment Returns a pointer to a segment.

  index_t n The index of the segment.
  ++++++++++++++++++++++++++++++++++++++*/

PythonSegment *PythonNode::get_segment(index_t n)
{
 fill_segments();

 if(n > segments.size())
    return NULL;

 return pydatabase->GetSegment(segments[n]);
}


/*++++++++++++++++++++++++++++++++++++++
  When acting as a list return the selected item from the iterator.

  template<> PythonNode *PythonDatabaseIter<PythonNode>::__getitem__ Returns a pointer to a node.

  index_t n The index of the node.
  ++++++++++++++++++++++++++++++++++++++*/

template<> PythonNode *PythonDatabaseIter<PythonNode>::__getitem__(index_t n)
{
 return pydatabase->GetNode(n);
}


/*++++++++++++++++++++++++++++++++++++++
  When acting as a list return the selected item from the iterator.

  template<> PythonSegment *PythonDatabaseIter<PythonSegment>::__getitem__ Returns a pointer to a segment.

  index_t n The index of the segment.
  ++++++++++++++++++++++++++++++++++++++*/

template<> PythonSegment *PythonDatabaseIter<PythonSegment>::__getitem__(index_t n)
{
 return pydatabase->GetSegment(n);
}


/*++++++++++++++++++++++++++++++++++++++
  When acting as a list return the selected item from the iterator.

  template<> PythonWay *PythonDatabaseIter<PythonWay>::__getitem__ Returns a pointer to a way.

  index_t n The index of the way.
  ++++++++++++++++++++++++++++++++++++++*/

template<> PythonWay *PythonDatabaseIter<PythonWay>::__getitem__(index_t n)
{
 return pydatabase->GetWay(n);
}


/*++++++++++++++++++++++++++++++++++++++
  When acting as a list return the selected item from the iterator.

  template<> PythonRelation *PythonDatabaseIter<PythonRelation>::__getitem__ Returns a pointer to a relation.

  index_t n The index of the relation.
  ++++++++++++++++++++++++++++++++++++++*/

template<> PythonRelation *PythonDatabaseIter<PythonRelation>::__getitem__(index_t n)
{
 return pydatabase->GetRelation(n);
}


/*++++++++++++++++++++++++++++++++++++++
  When acting as a list return the selected item from the iterator.

  template<> PythonSegment *PythonNodeIter<PythonSegment>::__getitem__ Returns a pointer to a segment.

  index_t n The index of the segment.
  ++++++++++++++++++++++++++++++++++++++*/

template<> PythonSegment *PythonNodeIter<PythonSegment>::__getitem__(index_t n)
{
 return pynode->get_segment(n);
}


/*++++++++++++++++++++++++++++++++++++++
  Convert a Python database to a viewable string.

  char *PythonDatabase::__str__ Returns a pointer to a statically allocated string.
  ++++++++++++++++++++++++++++++++++++++*/

char *PythonDatabase::__str__()
{
 static char tmp[256];

 if(prefix)
    sprintf(tmp, "Database(%s,%s)", dirname, prefix);
 else
    sprintf(tmp, "Database(%s)", dirname);

 return tmp;
}


/*++++++++++++++++++++++++++++++++++++++
  Convert a Python node to a viewable string.

  char *PythonNode::__str__ Returns a pointer to a statically allocated string.
  ++++++++++++++++++++++++++++++++++++++*/

char *PythonNode::__str__()
{
 static char tmp[64];

 sprintf(tmp, "Node(%" Pindex_t ")", id);

 return tmp;
}


/*++++++++++++++++++++++++++++++++++++++
  Convert a Python segment to a viewable string.

  char *PythonSegment::__str__ Returns a pointer to a statically allocated string.
  ++++++++++++++++++++++++++++++++++++++*/

char *PythonSegment::__str__()
{
 static char tmp[64];

 sprintf(tmp, "Segment(%" Pindex_t ")", id);

 return tmp;
}


/*++++++++++++++++++++++++++++++++++++++
  Convert a Python way to a viewable string.

  char *PythonWay::__str__ Returns a pointer to a statically allocated string.
  ++++++++++++++++++++++++++++++++++++++*/

char *PythonWay::__str__()
{
 static char tmp[64];

 sprintf(tmp, "Way(%" Pindex_t ")", id);

 return tmp;
}


/*++++++++++++++++++++++++++++++++++++++
  Convert a Python relation to a viewable string.

  char *PythonRelation::__str__ Returns a pointer to a statically allocated string.
  ++++++++++++++++++++++++++++++++++++++*/

char *PythonRelation::__str__()
{
 static char tmp[64];

 sprintf(tmp, "Relation(%" Pindex_t ")", id);

 return tmp;
}
