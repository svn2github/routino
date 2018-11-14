/***************************************
 Python database interface definition.

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


/* Name the module 'database' in the 'routino' package */

%module(package="routino.database") database


/* Include the 'database.hh' header file in the auto-generated code */

%{
#include "database.hh"
%}


/* Typemaps for the special integer types used by Routino */

%typemap(in)  index_t      { $1      = PyInt_AsLong($input); }
%typemap(out) index_t      { $result = PyInt_FromLong($1);   }

%typemap(in)  transport_t  { $1      = PyInt_AsLong($input); }
%typemap(out) transport_t  { $result = PyInt_FromLong($1);   }

%typemap(in)  transports_t { $1      = PyInt_AsLong($input); }
%typemap(out) transports_t { $result = PyInt_FromLong($1);   }

%typemap(in)  nodeflags_t  { $1      = PyInt_AsLong($input); }
%typemap(out) nodeflags_t  { $result = PyInt_FromLong($1);   }

%typemap(in)  highway_t    { $1      = PyInt_AsLong($input); }
%typemap(out) highway_t    { $result = PyInt_FromLong($1);   }

%typemap(in)  properties_t { $1      = PyInt_AsLong($input); }
%typemap(out) properties_t { $result = PyInt_FromLong($1);   }

%typemap(in)  distance_t   { $1      = PyInt_AsLong($input); }
%typemap(out) distance_t   { $result = PyInt_FromLong($1);   }


/* Exception handling for the iterators */

%exception PythonDatabaseIter<PythonNode>::__next__ {
  $action
  if (!result)
    {
     PyErr_SetString(PyExc_StopIteration, "End of iterator");
     return NULL;
    }
}

%exception PythonDatabaseIter<PythonSegment>::__next__ {
  $action
  if (!result)
    {
     PyErr_SetString(PyExc_StopIteration, "End of iterator");
     return NULL;
    }
}

%exception PythonDatabaseIter<PythonWay>::__next__ {
  $action
  if (!result)
    {
     PyErr_SetString(PyExc_StopIteration, "End of iterator");
     return NULL;
    }
}

%exception PythonDatabaseIter<PythonRelation>::__next__ {
  $action
  if (!result)
    {
     PyErr_SetString(PyExc_StopIteration, "End of iterator");
     return NULL;
    }
}

%exception PythonNodeIter<PythonSegment>::__next__ {
  $action
  if (!result)
    {
     PyErr_SetString(PyExc_StopIteration, "End of iterator");
     return NULL;
    }
}


/* Rename the internal data types to remove the 'Python' prefix */

%rename("Database") "PythonDatabase";

%rename("Node")     "PythonNode";
%rename("Segment")  "PythonSegment";
%rename("Way")      "PythonWay";
%rename("Relation") "PythonRelation";


/* Ignore most of the constructors */

%ignore PythonDatabase::PythonDatabase;

%ignore PythonNode::PythonNode;
%ignore PythonSegment::PythonSegment;
%ignore PythonWay::PythonWay;
%ignore PythonRelation::PythonRelation;

%ignore PythonDatabaseIter<PythonNode>::PythonDatabaseIter;
%ignore PythonDatabaseIter<PythonSegment>::PythonDatabaseIter;
%ignore PythonDatabaseIter<PythonWay>::PythonDatabaseIter;
%ignore PythonDatabaseIter<PythonRelation>::PythonDatabaseIter;

%ignore PythonNodeIter<PythonSegment>::PythonNodeIter;


/* Mark the functions that create new objects so they can be garbage collected */

%newobject LoadDatabase;

%newobject PythonDatabase::GetNode;
%newobject PythonDatabase::GetSegment;
%newobject PythonDatabase::GetWay;
%newobject PythonDatabase::GetRelation;

%newobject PythonDatabase::Nodes;
%newobject PythonDatabase::Segments;
%newobject PythonDatabase::Ways;
%newobject PythonDatabase::Relations;

%newobject PythonNode::Segments;

%newobject PythonSegment::Node1;
%newobject PythonSegment::Node2;
%newobject PythonSegment::Way;

%newobject PythonRelation::FromSegment;
%newobject PythonRelation::ViaNode;
%newobject PythonRelation::ToSegment;
%newobject PythonRelation::FromWay;
%newobject PythonRelation::ToWay;
%newobject PythonRelation::FromNode;
%newobject PythonRelation::ToNode;

%newobject PythonDatabaseIter<PythonNode>::__getitem__;
%newobject PythonDatabaseIter<PythonNode>::__next__;

%newobject PythonDatabaseIter<PythonSegment>::__getitem__;
%newobject PythonDatabaseIter<PythonSegment>::__next__;

%newobject PythonDatabaseIter<PythonWay>::__getitem__;
%newobject PythonDatabaseIter<PythonWay>::__next__;

%newobject PythonDatabaseIter<PythonRelation>::__getitem__;
%newobject PythonDatabaseIter<PythonRelation>::__next__;

%newobject PythonNodeIter<PythonSegment>::__getitem__;
%newobject PythonNodeIter<PythonSegment>::__next__;


/* Ignore most things from the types.h file except the enumerations */

%ignore M_PI;
%ignore NWAYPOINTS;
%ignore LAT_LONG_SCALE;
%ignore LAT_LONG_BIN;

%ignore kph_to_speed;
%ignore tonnes_to_weight;
%ignore metres_to_height;
%ignore metres_to_width;
%ignore metres_to_length;

%ignore HighwayType;
%ignore TransportType;
%ignore PropertyType;
%ignore HighwayName;
%ignore TransportName;
%ignore PropertyName;
%ignore HighwaysNameList;
%ignore AllowedNameList;
%ignore PropertiesNameList;
%ignore HighwayList;
%ignore TransportList;
%ignore PropertyList;


/* Use the 'database.hh' header file to generate the wrapper (everything is read-only) */

%immutable;

%include "database.hh"
%include "../src/types.h"


/* Declare the specific templates */

%template(DatabaseNodeIter)     PythonDatabaseIter<PythonNode>;
%template(DatabaseSegmentIter)  PythonDatabaseIter<PythonSegment>;
%template(DatabaseWayIter)      PythonDatabaseIter<PythonWay>;
%template(DatabaseRelationIter) PythonDatabaseIter<PythonRelation>;

%template(NodeSegmentIter)  PythonNodeIter<PythonSegment>;
