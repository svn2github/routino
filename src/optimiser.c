/***************************************
 $Header: /home/amb/CVS/routino/src/optimiser.c,v 1.10 2009-01-11 09:28:31 amb Exp $

 Routing optimiser.
 ******************/ /******************
 Written by Andrew M. Bishop

 This file Copyright 2008,2009 Andrew M. Bishop
 It may be distributed under the GNU Public License, version 2, or
 any higher version.  See section COPYING of the GNU Public license
 for conditions under which this file may be redistributed.
 ***************************************/


#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "nodes.h"
#include "ways.h"
#include "segments.h"
#include "results.h"
#include "functions.h"


#define INCREMENT 1024
#define NBINS     256


/*+ A queue results. +*/
typedef struct _Queue
{
 uint32_t alloced;              /*+ The amount of space allocated for results in the array +*/
 uint32_t number;               /*+ The number of occupied results in the array +*/
 Result *queue[1024];           /*+ An array of results whose size is not
                                    necessarily limited to 1024 (i.e. may
                                    overflow the end of this structure). +*/
}
 Queue;


/*+ The queue of nodes. +*/
Queue *OSMQueue=NULL;

/*+ Print the progress? +*/
int print_progress=1;


/* Functions */

static void insert_in_queue(Result *result);


/*++++++++++++++++++++++++++++++++++++++
  Find the optimum route between two nodes.

  Results *FindRoute Returns a set of results.

  Nodes *nodes The set of nodes to use.

  Segments *segments The set of segments to use.

  node_t start The start node.

  node_t finish The finish node.
  ++++++++++++++++++++++++++++++++++++++*/

Results *FindRoute(Nodes *nodes,Segments *segments,node_t start,node_t finish)
{
 Results *results;
 Node *Start,*Finish;
 node_t node2;
 Node *Node1,*Node2;
 HalfResult shortest1,quickest1;
 HalfResult shortest2,quickest2;
 HalfResult shortestfinish,quickestfinish;
 distance_t totalcrow,crow;
 Result *result1,*result2;
 Segment *segment;
 int nresults=0;

 /* Set up the finish conditions */

 shortestfinish.distance=~0;
 shortestfinish.duration=~0;
 quickestfinish.distance=~0;
 quickestfinish.duration=~0;

 /* Work out the distance as the crow flies */

 Start=FindNode(nodes,start);
 Finish=FindNode(nodes,finish);

 totalcrow=Distance(Start,Finish);

 /* Insert the first node into the queue */

 results=NewResultsList();

 result1=InsertResult(results,start);

 result1->node=start;
 result1->Node=Start;
 result1->shortest.Prev=NULL;
 result1->shortest.distance=0;
 result1->shortest.duration=0;
 result1->quickest.Prev=NULL;
 result1->quickest.distance=0;
 result1->quickest.duration=0;

 insert_in_queue(result1);

 /* Loop across all nodes in the queue */

 while(OSMQueue->number>0)
   {
    result1=OSMQueue->queue[--OSMQueue->number];
    Node1=result1->Node;
    shortest1.distance=result1->shortest.distance;
    shortest1.duration=result1->shortest.duration;
    quickest1.distance=result1->quickest.distance;
    quickest1.duration=result1->quickest.duration;

    segment=FindFirstSegment(segments,Node1->id);

    while(segment)
      {
       node2=segment->node2;

       if(segment->distance==(distance_short_t)~0 ||
          segment->duration==(distance_short_t)~0)
          goto endloop;

       shortest2.distance=shortest1.distance+segment->distance;
       shortest2.duration=shortest1.duration+segment->duration;
       quickest2.distance=quickest1.distance+segment->distance;
       quickest2.duration=quickest1.duration+segment->duration;

       result2=FindResult(results,node2);
       if(result2)
          Node2=result2->Node;
       else
          Node2=FindNode(nodes,node2);

       crow=Distance(Node2,Finish);

       if((crow+shortest2.distance)>(km_to_distance(10)+1.4*totalcrow))
          goto endloop;

       if(shortest2.distance>shortestfinish.distance && quickest2.duration>quickestfinish.duration)
          goto endloop;

       if(!result2)                         /* New end node */
         {
          result2=InsertResult(results,node2);
          result2->node=node2;
          result2->Node=Node2;
          result2->shortest.Prev=Node1;
          result2->shortest.distance=shortest2.distance;
          result2->shortest.duration=shortest2.duration;
          result2->quickest.Prev=Node1;
          result2->quickest.distance=quickest2.distance;
          result2->quickest.duration=quickest2.duration;

          nresults++;

          if(node2==finish)
            {
             shortestfinish.distance=shortest2.distance;
             shortestfinish.duration=shortest2.duration;
             quickestfinish.distance=quickest2.distance;
             quickestfinish.duration=quickest2.duration;
            }
          else
             insert_in_queue(result2);
         }
       else
         {
          if(shortest2.distance<result2->shortest.distance ||
             (shortest2.distance==result2->shortest.distance &&
              shortest2.duration<result2->shortest.duration)) /* New end node is shorter */
            {
             result2->shortest.Prev=Node1;
             result2->shortest.distance=shortest2.distance;
             result2->shortest.duration=shortest2.duration;

             if(node2==finish)
               {
                shortestfinish.distance=shortest2.distance;
                shortestfinish.duration=shortest2.duration;
               }
             else
                insert_in_queue(result2);
            }

          if(quickest2.duration<result2->quickest.duration ||
             (quickest2.duration==result2->quickest.duration &&
              quickest2.distance<result2->quickest.distance)) /* New end node is quicker */
            {
             result2->quickest.Prev=Node1;
             result2->quickest.distance=quickest2.distance;
             result2->quickest.duration=quickest2.duration;

             if(node2==finish)
               {
                quickestfinish.distance=quickest2.distance;
                quickestfinish.duration=quickest2.duration;
               }
             else
                insert_in_queue(result2);
            }
         }

      endloop:

       segment=FindNextSegment(segments,segment);
      }

    if(print_progress && !(nresults%1000))
      {
       printf("\rRouting: End Nodes=%d Queue=%d Journey=%.1fkm,%.0fmin  ",nresults,OSMQueue->number,
              distance_to_km(shortest2.distance),duration_to_minutes(quickest2.duration));
       fflush(stdout);
      }
   }

 if(print_progress)
   {
    printf("\rRouted: End Nodes=%d Shortest=%.1fkm,%.0fmin Quickest=%.1fkm,%.0fmin\n",nresults,
           distance_to_km(shortestfinish.distance),duration_to_minutes(shortestfinish.duration),
           distance_to_km(quickestfinish.distance),duration_to_minutes(quickestfinish.duration));
    fflush(stdout);
   }

 /* Reverse the results */

 result2=FindResult(results,finish);

 do
   {
    if(result2->shortest.Prev)
      {
       node_t node1=result2->shortest.Prev->id;

       result1=FindResult(results,node1);

       result1->shortest.Next=result2->Node;

       result2=result1;
      }
    else
       result2=NULL;
   }
 while(result2);

 result2=FindResult(results,finish);

 do
   {
    if(result2->quickest.Prev)
      {
       node_t node1=result2->quickest.Prev->id;

       result1=FindResult(results,node1);

       result1->quickest.Next=result2->Node;

       result2=result1;
      }
    else
       result2=NULL;
   }
 while(result2);

 return(results);
}


/*++++++++++++++++++++++++++++++++++++++
  Print the optimum route between two nodes.

  Results *Results The set of results to print.

  Nodes *nodes The list of nodes.

  Segments *segments The set of segments to use.

  Ways *ways The list of ways.

  node_t start The start node.

  node_t finish The finish node.
  ++++++++++++++++++++++++++++++++++++++*/

void PrintRoute(Results *results,Nodes *nodes,Segments *segments,Ways *ways,node_t start,node_t finish)
{
 FILE *file;
 Result *result;

 /* Print the result for the shortest route */

 file=fopen("shortest.txt","w");

 result=FindResult(results,start);

 do
   {
    if(result->shortest.Next)
      {
       Segment *segment;
       Way *way;

       segment=FindFirstSegment(segments,result->Node->id);
       while(segment->node2!=result->shortest.Next->id)
          segment=FindNextSegment(segments,segment);

       way=FindWay(ways,segment->way);

       fprintf(file,"%9.5f %9.5f %9d %5.3f %5.2f %3d %s\n",result->Node->latitude,result->Node->longitude,result->node,
               distance_to_km(segment->distance),duration_to_minutes(segment->duration),
               (way->limit?way->limit:way->speed),WayName(ways,way));

       result=FindResult(results,result->shortest.Next->id);
      }
    else
      {
       fprintf(file,"%9.5f %9.5f %9d\n",result->Node->latitude,result->Node->longitude,result->node);

       result=NULL;
      }
   }
 while(result);

 fclose(file);

 /* Print the result for the quickest route */

 file=fopen("quickest.txt","w");

 result=FindResult(results,start);

 do
   {
    if(result->quickest.Next)
      {
       Segment *segment;
       Way *way;

       segment=FindFirstSegment(segments,result->Node->id);
       while(segment->node2!=result->quickest.Next->id)
          segment=FindNextSegment(segments,segment);

       way=FindWay(ways,segment->way);

       fprintf(file,"%9.5f %9.5f %9d %5.3f %5.2f %3d %s\n",result->Node->latitude,result->Node->longitude,result->node,
               distance_to_km(segment->distance),duration_to_minutes(segment->duration),
               (way->limit?way->limit:way->speed),WayName(ways,way));

       result=FindResult(results,result->quickest.Next->id);
      }
    else
      {
       fprintf(file,"%9.5f %9.5f %9d\n",result->Node->latitude,result->Node->longitude,result->node);

       result=NULL;
      }
   }
 while(result);

 fclose(file);
}


/*++++++++++++++++++++++++++++++++++++++
  Find all routes from a specified node to any node in the specified list.

  Results *FindRoute2 Returns a set of results.

  Nodes *nodes The set of nodes to use.

  Segments *segments The set of segments to use.

  node_t start The start node.

  Nodes *finish The finishing nodes.
  ++++++++++++++++++++++++++++++++++++++*/

Results *FindRoutes(Nodes *nodes,Segments *segments,node_t start,Nodes *finish)
{
 Results *results;
 Node *Start;
 node_t node2;
 Node *Node1,*Node2;
 HalfResult shortest1,quickest1;
 HalfResult shortest2,quickest2;
 Result *result1,*result2;
 Segment *segment;
 int nresults=0;

 Start=FindNode(nodes,start);

 /* Insert the first node into the queue */

 results=NewResultsList();

 result1=InsertResult(results,start);

 result1->node=start;
 result1->Node=Start;
 result1->shortest.Prev=NULL;
 result1->shortest.Next=NULL;
 result1->shortest.distance=0;
 result1->shortest.duration=0;
 result1->quickest.Prev=NULL;
 result1->quickest.Next=NULL;
 result1->quickest.distance=0;
 result1->quickest.duration=0;

 insert_in_queue(result1);

 /* Loop across all nodes in the queue */

 while(OSMQueue->number>0)
   {
    result1=OSMQueue->queue[--OSMQueue->number];
    Node1=result1->Node;
    shortest1.distance=result1->shortest.distance;
    shortest1.duration=result1->shortest.duration;
    quickest1.distance=result1->quickest.distance;
    quickest1.duration=result1->quickest.duration;

    segment=FindFirstSegment(segments,Node1->id);

    while(segment)
      {
       node2=segment->node2;
       Node2=FindNode(nodes,node2);

       if(segment->distance==(distance_short_t)~0 ||
          segment->duration==(distance_short_t)~0)
          goto endloop;

       shortest2.distance=shortest1.distance+segment->distance;
       shortest2.duration=shortest1.duration+segment->duration;
       quickest2.distance=quickest1.distance+segment->distance;
       quickest2.duration=quickest1.duration+segment->duration;

       result2=FindResult(results,node2);

       if(!result2)                         /* New end node */
         {
          result2=InsertResult(results,node2);
          result2->node=node2;
          result2->Node=Node2;
          result2->shortest.Prev=Node1;
          result2->shortest.Next=NULL;
          result2->shortest.distance=shortest2.distance;
          result2->shortest.duration=shortest2.duration;
          result2->quickest.Prev=Node1;
          result2->quickest.Next=NULL;
          result2->quickest.distance=quickest2.distance;
          result2->quickest.duration=quickest2.duration;

          nresults++;

          if(!FindNode(finish,node2))
             insert_in_queue(result2);
         }
       else
         {
          if(shortest2.distance<result2->shortest.distance ||
             (shortest2.distance==result2->shortest.distance &&
              shortest2.duration<result2->shortest.duration)) /* New end node is shorter */
            {
             result2->shortest.Prev=Node1;
             result2->shortest.distance=shortest2.distance;
             result2->shortest.duration=shortest2.duration;

             if(!FindNode(finish,node2))
                insert_in_queue(result2);
            }

          if(quickest2.duration<result2->quickest.duration ||
             (quickest2.duration==result2->quickest.duration &&
              quickest2.distance<result2->quickest.distance)) /* New end node is quicker */
            {
             result2->quickest.Prev=Node1;
             result2->quickest.distance=quickest2.distance;
             result2->quickest.duration=quickest2.duration;

             if(!FindNode(finish,node2))
                insert_in_queue(result2);
            }
         }

      endloop:

       segment=FindNextSegment(segments,segment);
      }
   }

 return(results);
}


/*++++++++++++++++++++++++++++++++++++++
  Print the optimum route between two nodes.

  Results *Results The set of results to print.

  Nodes *nodes The list of nodes.

  Segments *segments The set of segments to use.

  Ways *ways The list of ways.

  Nodes *junctions The list of junctions.

  Segments *supersegments The list of supersegments.

  node_t start The start node.

  node_t finish The finish node.
  ++++++++++++++++++++++++++++++++++++++*/

void PrintRoutes(Results *results,Nodes *nodes,Segments *segments,Ways *ways,Nodes *junctions,Segments *supersegments,node_t start,node_t finish)
{
 Result *result1,*result2,*result3,*result4;
 Results *combined;

 combined=NewResultsList();

 print_progress=0;

 /* Sort out the shortest */

 result1=FindResult(results,start);

 do
   {
    result3=InsertResult(combined,result1->node);

    result3->node=result1->node;
    result3->Node=result1->Node;
    result3->shortest=result1->shortest;

    if(result1->shortest.Next)
      {
       Results *results2=FindRoute(nodes,segments,result1->node,result1->shortest.Next->id);

       result2=FindResult(results2,result1->node);

       do
         {
          if(result2->shortest.Prev && result2->shortest.Prev->id==result3->node)
             result3->shortest.Next=result2->Node;

          if(result2->shortest.Prev && result2->shortest.Next)
            {
             result4=InsertResult(combined,result2->node);

             result4->node=result2->node;
             result4->Node=result2->Node;
             result4->shortest=result2->shortest;
            }

          if(result2->shortest.Next)
             result2=FindResult(results2,result2->shortest.Next->id);
          else
             result2=NULL;
         }
       while(result2);

       FreeResultsList(results2);

       result1=FindResult(results,result1->shortest.Next->id);
      }
    else
       result1=NULL;
   }
 while(result1);

 /* Sort out the quickest */

 result1=FindResult(results,start);

 do
   {
    result3=FindResult(combined,result1->node);

    if(!result3)
       result3=InsertResult(combined,result1->node);

    result3->node=result1->node;
    result3->Node=result1->Node;
    result3->quickest=result1->quickest;

    if(result1->quickest.Next)
      {
       Results *results2=FindRoute(nodes,segments,result1->node,result1->quickest.Next->id);

       result2=FindResult(results2,result1->node);

       do
         {
          if(result2->quickest.Prev && result2->quickest.Prev->id==result3->node)
             result3->quickest.Next=result2->Node;

          if(result2->quickest.Prev && result2->quickest.Next)
            {
             result4=FindResult(combined,result2->node);

             if(!result4)
                result4=InsertResult(combined,result2->node);

             result4->node=result2->node;
             result4->Node=result2->Node;
             result4->quickest=result2->quickest;
            }

          if(result2->quickest.Next)
             result2=FindResult(results2,result2->quickest.Next->id);
          else
             result2=NULL;
         }
       while(result2);

       FreeResultsList(results2);

       result1=FindResult(results,result1->quickest.Next->id);
      }
    else
       result1=NULL;
   }
 while(result1);

 /* Now print out the result normally */

 print_progress=1;

 PrintRoute(combined,nodes,segments,ways,start,finish);
}


/*++++++++++++++++++++++++++++++++++++++
  Insert an item into the queue in the right order.

  Result *result The result to insert into the queue.
  ++++++++++++++++++++++++++++++++++++++*/

static void insert_in_queue(Result *result)
{
 int start;
 int end;
 int mid;
 int insert=-1;

 /* Check that the whole thing is allocated. */

 if(!OSMQueue)
   {
    OSMQueue=(Queue*)malloc(sizeof(Queue)-sizeof(OSMQueue->queue)+INCREMENT*sizeof(Result*));

    OSMQueue->alloced=INCREMENT;
    OSMQueue->number=0;
   }

 /* Check that the arrays have enough space. */

 if(OSMQueue->number==OSMQueue->alloced)
   {
    OSMQueue->alloced+=INCREMENT;
    OSMQueue=(Queue*)realloc((void*)OSMQueue,sizeof(Queue)-sizeof(OSMQueue->queue)+OSMQueue->alloced*sizeof(Result*));
   }

 /* Binary search - search key may not match, new insertion point required
  *
  *  # <- start  |  Check mid and move start or end if it doesn't match
  *  #           |
  *  #           |  Since there may not be an exact match we must set end=mid
  *  # <- mid    |  or start=mid because we know that mid doesn't match.
  *  #           |
  *  #           |  Eventually end=start+1 and the insertion point is before
  *  # <- end    |  end (since it cannot be before the initial start or end).
  */

 start=0;
 end=OSMQueue->number-1;

 if(OSMQueue->number==0)                                                      /* There is nothing in the queue */
    insert=start;
 else if(result->shortest.distance>OSMQueue->queue[start]->shortest.distance) /* Check key is not before start */
    insert=start;
 else if(result->shortest.distance<OSMQueue->queue[end]->shortest.distance)   /* Check key is not after end */
    insert=end+1;
 else
   {
    do
      {
       mid=(start+end)/2;                                                         /* Choose mid point */

       if(OSMQueue->queue[mid]->shortest.distance>result->shortest.distance)      /* Mid point is too low */
          start=mid;
       else if(OSMQueue->queue[mid]->shortest.distance<result->shortest.distance) /* Mid point is too high */
          end=mid;
       else                                                                       /* Mid point is correct */
         {
          if(OSMQueue->queue[mid]==result)
             return;

          insert=mid;
          break;
         }
      }
    while((end-start)>1);

    if(insert==-1)
       insert=end;
   }

 /* Shuffle the array up */

 if(insert!=OSMQueue->number)
    memmove(&OSMQueue->queue[insert+1],&OSMQueue->queue[insert],(OSMQueue->number-insert)*sizeof(Result*));

 /* Insert the new entry */

 OSMQueue->queue[insert]=result;

 OSMQueue->number++;
}
