/* 
 * File:   stRTree.h
 * Author: aocsa
 *
 * Created on January 16, 2007, 3:07 PM
 */

#ifndef _stRTree_H
#define	_stRTree_H

#include <arboretum/stPageManager.h>
#include <arboretum/stTypes.h>
#include <arboretum/stUtil.h>
#include <arboretum/stResult.h>
#include <arboretum/stGenericPriorityQueue.h>

#include "stRLogicNode.h"
#include "stSubTreeInfo.h"

#define R_ORDER(A, B) (floor(((A-NODE_HEAD_SIZE)/double(ENTRY_SIZE+B))) - 1)
// this is used to set the initial size of the dynamic queue
#define STARTVALUEQUEUE 200
// this is used to set the increment size of the dynamic queue
#define INCREMENTVALUEQUEUE 200



//=============================================================================
// Class template stRTree
//-----------------------------------------------------------------------------
/**
* @author Alexander Ocsa . (alexocsa@unsa.edu.pe)
* @todo More documentation.??
* @todo Finish the implementation.??
* @version 1.0
*/

class stRTree {
public:
    typedef stMBR           tObject;
	typedef stResult<tObject> tResult;
	typedef stRLogicNode<tObject> tLogicNode;
	
    typedef stSubTreeInfo<tObject> tSubTreeInfo;
	static int NumberOfDC;

	struct stQueryPriorityQueueValue{
	   /**
	   * ID of the node.
	   */
	   tLogicNode* PageID;
   
	   /**
	   * Radius of the node.
	   */
	   stDistance Radius;
   
	   /**
	   * Operator = . 
	   *
	   * @param v Another instance of this class.
	   */
	   const stQueryPriorityQueueValue & operator = (const stQueryPriorityQueueValue & v){
      	  this->PageID = v.PageID;
		  this->Radius = v.Radius;
		  return *this;
	   }//end operator =
	}stQueryPQueueValue;

public:
    stRTree(int order);
    
    virtual   ~stRTree();    
    bool	  Add(tObject* data);
	int		  Remove(tObject* obj);
    bool	  UpdateData(tObject* oldValue, tObject* newValue);
	int		  Remove(tLogicNode* currNode, tObject* obj);
	tResult*  IntersectionQuery(tObject* data);
    tResult*  NearestQuery(tObject * sample, stCount k, bool tie = false);
    void      Print(std::ostream& out) {
		if(GetRoot() != 0) {
			out << "----------------------------------------\n";
			this->Print(out, GetRoot(), 0);
			out << "----------------------------------------\n\n";
		}
	}
	void      Print(std::ostream& out, tLogicNode* currNode, int level); 

	stCount GetNumberOfObjects() {
		return HeaderInfo->ObjectCount; 
	}

protected:
    enum stInsertAction{ NO_ACT,
                         CHANGE_REP,
                         PROMOTION,
                         REMOVE_FARTHEST,
                         REMOVE_NODE,
						 NOT_FOUND
    };

    typedef stRPriorityQueue < stDistance, stCount > tPriorityQueue;
    typedef stDynamicRPriorityQueue < stDistance, stQueryPriorityQueueValue > tDynamicPriorityQueue;
    typedef stGenericPriorityHeap < tObject > tPGenericHeap;

private:
    void WriteHeader();
    void LoadHeader();
    void DefaultHeader();
    bool RecursiveUpdateData(tLogicNode* currNode, tObject* oldValue, tObject* newValue, tSubTreeInfo* promo);
	int  RecursiveInsert(tLogicNode* currNode, tSubTreeInfo& newSubTree, tLogicNode* dataNode, tSubTreeInfo* promo);
	int  RecursiveRemove(tLogicNode* currNode, tObject* obj, tSubTreeInfo* promo);
  
	void AddNewRoot(tSubTreeInfo*  promo);
    void Split(tLogicNode* currNode, tSubTreeInfo*  promo);

	int ChooseSubTree(tLogicNode* node, tSubTreeInfo &newSubtree);

	void NearestQuery(tResult * result,
         tObject * sample, stDistance rangeK, stCount k,
         tDynamicPriorityQueue * queue);

	tLogicNode* GetRoot() {
        return this->HeaderInfo->Root;
    }
	void SetRoot(tLogicNode* id) {
		this->HeaderInfo->Root = id;
		this->HeaderUpdate = true;
	}
    stSize GetOrder() {
		return this->HeaderInfo->Order;
	}
	stSize GetNumberOfClusters() {
		return this->HeaderInfo->NumberOfClusters;
	}

	stMBR*	GetFirstMBR(tLogicNode* currNode); 


private:
    
    struct stRTreeHeader {
        int			Order;
		tObject*	TreeMBR;
		int			NumberOfClusters;
		int         SplitMethod;
        int         ChooseMethod;
        tLogicNode* Root;
        stCount     MaxOccupation;
        stCount     ObjectCount;
        stSize      NodeCount;
        bool        ReInsertObjects;
        stCount     Height;
    };    

public:
    enum tChooseMethod {
        ORIGINAL_RTREE_CHOOSE
    };
    enum tSplitMethod {
        ORIGINAL_RTREE_SPLIT
    };
    
    
private:
    stPageManager*	myPageManager;
    stRTreeHeader*	HeaderInfo;
    stPage*			HeaderPage;    
    bool            HeaderUpdate;
};

//-------------------------------------------------------------------------------------

stRTree::stRTree(int order)
{
		
    HeaderInfo = NULL;
    HeaderPage = NULL;
    
    LoadHeader();
    
	DefaultHeader();
    // Allocate Collection for ReInsert

	this->HeaderInfo->Order = order; 
	
}


stRTree::~stRTree()
{
   
}


void stRTree::WriteHeader() 
{

}


void stRTree::LoadHeader() 
{
	HeaderInfo = new stRTreeHeader;
}


void stRTree::DefaultHeader() 
{
    //this->HeaderPage->Clear();
	this->HeaderInfo->TreeMBR = NULL;
    this->HeaderInfo->Height = 0;
    this->HeaderInfo->ChooseMethod = 0;
    this->HeaderInfo->MaxOccupation = 0;
    this->HeaderInfo->NodeCount = 0;
    this->HeaderInfo->ObjectCount = 0;
	this->HeaderInfo->NumberOfClusters = 2;
    this->HeaderInfo->ReInsertObjects = false;
    this->HeaderInfo->Root = 0;
    this->HeaderInfo->SplitMethod = 0;
    this->HeaderUpdate = true;        
}

bool	stRTree::Add(tObject* data)
{
	int insertIdx;
    tLogicNode* rootNode;
	tLogicNode* dataNode = NULL;

    if ( this->GetRoot() == NULL) {
       	#ifdef __stDEBUG__
			std::cout << "first object\n";
		#endif

		rootNode = new tLogicNode(this->GetOrder());
		rootNode->SetParent(NULL);
        insertIdx = rootNode->AddEntry(data, 0, 0);
		((stPoint*)(rootNode->GetObject(insertIdx)))->SetNode(rootNode);	

		#ifdef __stDEBUG__
		if(insertIdx < 0) {
			throw page_size_error("The page size is too small to store the first object");
		}
		#endif // __stDEBUG__
		
		this->SetRoot(rootNode);
		this->HeaderInfo->Height = 1;

		dataNode = rootNode;
    }
    else {
        tSubTreeInfo subTree;
        subTree.Rep = data;
        subTree.Height = 0;
        subTree.NObjects = 0;
        subTree.RootID = 0;
        
        tSubTreeInfo* promo = new tSubTreeInfo[2];
        promo[0].Rep = NULL;
        promo[1].Rep = NULL;
        
		#ifdef __stDEBUG__
			std::cout << "first, recursive insert\n";
		#endif
        if( PROMOTION == this->RecursiveInsert( this->GetRoot(), subTree, dataNode, promo) ){
            this->AddNewRoot(promo);       //this->HeaderInfo->Height++;
            this->HeaderUpdate = true;
        }
		this->HeaderInfo->TreeMBR = promo[0].Rep;			
	}
	this->HeaderInfo->ObjectCount++;
    return true;
}


int stRTree::RecursiveInsert(tLogicNode* currNode, tSubTreeInfo& newSubTree, tLogicNode* dataNode, tSubTreeInfo* promo)
{
    int idxSubTree, insertIdx;
    int idx, resultAction;
    tObject* subTreeObj;
    stMBR* mbr;
    tObject* tmpObj;
	
    idxSubTree = this->ChooseSubTree(currNode, newSubTree);
    
    if( idxSubTree >= 0) {
        subTreeObj = newSubTree.Rep;
        
		#ifdef __stDEBUG__
			std::cout << "recursive insert\n";
		#endif
        switch( this->RecursiveInsert(currNode->GetEntry(idxSubTree).PageID, newSubTree, dataNode, promo) ) {
            case NO_ACT:
				#ifdef __stDEBUG__
					std::cout << "update\n";
				#endif
				currNode->SetEntry(idxSubTree, promo[0].Rep, promo[0].NObjects, promo[0].RootID);
				promo[0].RootID->SetParent(currNode);
				
				mbr = GetFirstMBR(currNode); 
				for (idx = 1; idx < currNode->GetNumberOfEntries(); idx++) {
					tmpObj = currNode->GetObject(idx); 
					mbr = mbr->GetUnionMBR( (stMBR*)tmpObj );
				}
				promo[0].Rep = (tObject*)mbr;
				promo[0].NObjects = currNode->GetTotalObjectCount();
				promo[0].RootID = currNode->GetPageID();
				resultAction = NO_ACT;

				for (idx = 0; idx < currNode->GetNumberOfEntries(); idx++) {
					assert(currNode->GetEntry(idx).PageID->GetParent() == currNode);				
				}
				break;

            case PROMOTION:
				#ifdef __stDEBUG__
					std::cout << "promotion\n";
				#endif
				bool stop;
				
				idx = 0;
				currNode->SetEntry( idxSubTree, promo[idx].Rep,
												promo[idx].NObjects,
												promo[idx].RootID);

				promo[idx].RootID->SetParent(currNode);
			    
				idx++;
				stop = (idx == this->GetNumberOfClusters());
				while (!stop) {
					if(promo[idx].Rep != NULL) {
						insertIdx = currNode->AddEntry(promo[idx].Rep,
														promo[idx].NObjects,
														promo[idx].RootID);
						promo[idx].RootID->SetParent(currNode);

						if(insertIdx < 0) {
							stop = true;
						}
					}
					idx++;
					stop = (stop  || idx == this->GetNumberOfClusters());
				}				
				if( currNode->IsOverflow()) {
					this->Split(currNode, promo);

					for (idx = 0; idx < promo[0].RootID->GetNumberOfEntries(); idx++) {
						promo[0].RootID->GetEntry(idx).PageID->SetParent(promo[0].RootID);				
					}
					for (idx = 0; idx < promo[1].RootID->GetNumberOfEntries(); idx++) {
						promo[1].RootID->GetEntry(idx).PageID->SetParent(promo[1].RootID);				
					}

					resultAction = PROMOTION;
				}
				else {
					mbr = GetFirstMBR(currNode); 
					for (idx = 1; idx < currNode->GetNumberOfEntries(); idx++) {
						tmpObj = currNode->GetObject(idx); 
						mbr = mbr->GetUnionMBR( (stMBR*)tmpObj );
					}
					promo[0].Rep = (tObject*)mbr;
					promo[0].NObjects = currNode->GetTotalObjectCount();
					promo[0].RootID = currNode->GetPageID();
					resultAction = NO_ACT;
				}

				break;
        }
        
    }
    else {
		#ifdef __stDEBUG__
			std::cout << "add object\n";
		#endif
        insertIdx = currNode->AddEntry(newSubTree.Rep, 0, 0);
		if( currNode->IsOverflow()) {
			this->Split(currNode, promo);
			resultAction = PROMOTION;
		}
		else {
			((stPoint*)(currNode->GetObject(insertIdx)))->SetNode(currNode);	
			mbr = GetFirstMBR(currNode); 
			for (idx = 1; idx < currNode->GetNumberOfEntries(); idx++) {
				tmpObj = currNode->GetObject(idx); 
				mbr = mbr->GetUnionMBR( (stMBR*)tmpObj );
			}
			promo[0].Rep = (tObject*)mbr;
			promo[0].NObjects = currNode->GetTotalObjectCount();
			promo[0].RootID = currNode->GetPageID();
			resultAction = NO_ACT;
		}
    }
	
	#ifdef __stDEBUG__
		std::cout << "resultAction "<< resultAction << "\n";
	#endif
    return resultAction;
}

stMBR*	stRTree::GetFirstMBR(tLogicNode* currNode) {
	int idx;
	stHyperCube* mbr = new stHyperCube();
	for (idx = 0; idx <  currNode->GetObject(0)->GetDimension() ; idx++) {
		mbr->AddInterval(idx, currNode->GetObject(0)->GetBegin(idx), currNode->GetObject(0)->GetEnd(idx));
	}
	return mbr;
}

void stRTree::AddNewRoot(tSubTreeInfo* promo)
{
	int idx, insertIdx;
	tLogicNode* rootNode;
	
	rootNode = new tLogicNode(this->GetOrder());

	for (idx = 0; idx < this->GetNumberOfClusters(); idx++) {
		if(promo[idx].Rep) {
			insertIdx = rootNode->AddEntry(promo[idx].Rep, promo[idx].NObjects, promo[idx].RootID);
			promo[idx].RootID->SetParent(rootNode);
		}
	}
	
	this->HeaderInfo->Height++;
	this->SetRoot(rootNode);		
	rootNode->SetParent(NULL);

	tObject* tmpObj;
	tObject* mbr = GetFirstMBR(rootNode);
	for (idx = 1; idx < rootNode->GetNumberOfEntries(); idx++) {
		tmpObj = rootNode->GetObject(idx); 
		mbr = mbr->GetUnionMBR( (stMBR*)tmpObj );
	}
	promo[0].Rep = (tObject*)mbr;
}

void stRTree::Split(tLogicNode* oldNode,tSubTreeInfo*  promo)
{
	tLogicNode* newNode;
	tLogicNode* logicNode = new tLogicNode( oldNode->GetNumberOfEntries() );
	
	newNode = new tLogicNode(GetOrder());
	logicNode->AddNode(oldNode);
	oldNode->RemoveAll();
	logicNode->Distribute(oldNode, newNode, promo);
}



int stRTree::ChooseSubTree(tLogicNode* node, tSubTreeInfo &newSubtree)
{
	if(node->GetNumberOfEntries() == node->GetNumberOfFreeObjects() )
		return -1;
    
	int subTree = -1;
    int i;
    stDistance increased;
    stDistance min = MAXDOUBLE;
    tObject* tmpObj;
	for (i = 0; i < node->GetNumberOfEntries(); i++) {
		tmpObj = node->GetObject(i);
		stMBR* a = tmpObj->GetUnionMBR( newSubtree.Rep );
		increased = a->GetArea() - tmpObj->GetArea();
		if( fabs(increased) < min ) {
            min = fabs(increased);
            subTree = i;
        }
		delete a;
    }
	assert(subTree != -1);
    return subTree;
}

int stRTree::Remove(tLogicNode* childNode, tObject* obj)
{
	int idx;
	int result;
	tObject* mbr;
	tObject* tmpObj;
	tSubTreeInfo promo;
	tLogicNode* parentNode;
	for (idx = 0; idx < childNode->GetNumberOfEntries(); idx++) {
		if( obj->IsEqual(childNode->GetObject(idx)) )
			break;
	}
	if(idx < childNode->GetNumberOfEntries()) {
		childNode->RemoveEntry(idx);
		if( childNode->GetNumberOfEntries() == 0) {
			result = REMOVE_NODE;			
			promo.RootID = childNode->GetPageID();
		}
		else{
			mbr = GetFirstMBR(childNode); 
			for (idx = 1; idx < childNode->GetNumberOfEntries(); idx++) {
				tmpObj = childNode->GetObject(idx); 
				mbr = mbr->GetUnionMBR( (stMBR*)tmpObj );
			}
			promo.Rep = (tObject*)mbr;
			promo.NObjects = childNode->GetTotalObjectCount();
			promo.RootID = childNode->GetPageID();
			result = NO_ACT;
		}
	}
	else{
		result = NOT_FOUND;
	}
	if(result != NOT_FOUND) {
		childNode = promo.RootID;
		parentNode = promo.RootID->GetParent();		
		while (parentNode) {
			if(result == NO_ACT) {
				for (idx = 0; idx < parentNode->GetNumberOfEntries(); idx++) {
					if( parentNode->GetEntry(idx).PageID == childNode)
						break;
				}
				if( idx < parentNode->GetNumberOfEntries()) {
					parentNode->SetEntry(idx, promo.Rep, promo.NObjects, promo.RootID);
					mbr = GetFirstMBR(parentNode); 
					for (idx = 1; idx < parentNode->GetNumberOfEntries(); idx++) {
						tmpObj = parentNode->GetObject(idx); 
						mbr = mbr->GetUnionMBR( (stMBR*)tmpObj );
					}
					promo.Rep = (tObject*)mbr;
					promo.NObjects = parentNode->GetTotalObjectCount();
					promo.RootID = parentNode->GetPageID();
					result = NO_ACT;
				}
				else {
					std::cout << "error, noact, remove";break;
				}
			}
			else if(result == REMOVE_NODE) {
				for (idx = 0; idx < parentNode->GetNumberOfEntries(); idx++) {
					if( parentNode->GetEntry(idx).PageID == childNode)
						break;
				}
				if( idx < parentNode->GetNumberOfEntries()) {
					parentNode->RemoveEntry(idx);
					if( parentNode->GetNumberOfEntries() == 0) {
						promo.RootID = parentNode->GetPageID();
						result = REMOVE_NODE;
					}
					else {
						mbr = GetFirstMBR(parentNode); 
						for (idx = 1; idx < parentNode->GetNumberOfEntries(); idx++) {
							tmpObj = parentNode->GetObject(idx); 
							mbr = mbr->GetUnionMBR( (stMBR*)tmpObj );
						}
						promo.Rep = (tObject*)mbr;
						promo.NObjects = parentNode->GetTotalObjectCount();
						promo.RootID = parentNode->GetPageID();
						result = NO_ACT;
					}
				}
				else {
					std::cout << "error, remove_node, remove"; break;
				}
			}
			childNode = promo.RootID;
			parentNode = promo.RootID->GetParent();
		}
	}
	return result;
}

void stRTree::Print(std::ostream& out, tLogicNode* currNode, int level) 
{
	int idx;
	tObject* tmpObj;
	for (idx = currNode->GetNumberOfEntries() - 1; idx >= 0; idx--) {
		
		if(currNode->GetEntry(idx).PageID) {
			assert(currNode == currNode->GetEntry(idx).PageID->GetParent());
			this->Print(out, currNode->GetEntry(idx).PageID, level + 1);
		}
		tmpObj = currNode->GetObject(idx);
		
		for (int i = 0; i < DIMENSION;i++) {
			for (int k = 0; k < level ; k++) {
				out << "        ";
			}
			out << tmpObj->GetBegin(i) << " - " << tmpObj->GetEnd(i) << "\n";
		}
		if(currNode->GetEntry(idx).PageID == NULL){
			assert(((stPoint*)tmpObj)->GetNode() == currNode);
		}
		for (int k = 0; k < level ; k++) {
			out << "        ";
		}		
		//out << tmpObj->GetOID() << "|\n";
		out << "\n";
	}
}


stResult<stMBR>*  stRTree::NearestQuery(tObject * sample, stCount k, bool tie )
{
   tResult * result = new tResult();  // Create result
   tObject* tmpObj;
   tLogicNode * currNode;
   stDistance distance, distanceRepres;
   stDistance rangeK = MAXDOUBLE;
   stCount numberOfEntries, idx;
   stCount idxRep;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   tDynamicPriorityQueue * queue;

   // Set information for this query
   result->SetQueryInfo(sample->Clone(), tResult::KNEARESTQUERY, k, MAXDOUBLE, tie);
   // Let's search
   if (this->GetRoot() != 0){
      // Create the Global Priority Queue.
      queue = new tDynamicPriorityQueue(STARTVALUEQUEUE, INCREMENTVALUEQUEUE);

      // Read node...
      currNode = this->GetRoot();
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();
      // Rebuild the object
      //tmpObj.Unserialize(HeaderInfo->TreeMBR->Serialize(),
      //                   HeaderInfo->TreeMBR->GetSerializedSize());
      // Evaluate the distance.
      distanceRepres = MAXDOUBLE;

      // Is there any free objects in this node?
      if (currNode->GetNumberOfFreeObjects() > 0){
         // Now do it all free objects.
         for (idx = 0; idx < numberOfEntries; idx++) {
            // tests if this is a subtree.
            if (!currNode->GetEntry(idx).PageID) {
               // use of the triangle inequality to cut a subtree
               //if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <= rangeK){
				  tmpObj = currNode->GetObject(idx);
	               // is it a Representative?
	               //if (currNode->GetEntry(idx).Distance != 0) {
	                  // No, it is not a representative. Evaluate distance
	               distance = tmpObj->GetMinDist(sample);
				   stRTree::NumberOfDC++;
	               //}else{
	               //   distance = distanceRepres;
	               //}//end if

	               if (distance <= rangeK){
	                  // No, there is not a subtree. But this object qualifies.
	                  // Add the object.
	                  result->AddPair(tmpObj->Clone(), distance);
	                  // there is more than k elements?
	                  if (result->GetNumOfEntries() >= k){
	                     //cut if there is more than k elements
	                     result->Cut(k);
	                     //may I use this for performance?
	                     rangeK = result->GetMaximumDistance();
	                  }//end if
	               }//end if
               //}//end if
            }//end if
         }//end for
      }//end if

      // Now do it for subtrees.
      for (idx = 0; idx < numberOfEntries; idx++) {
         // tests if this is a subtree.
         if (currNode->GetEntry(idx).PageID){
            // use of the triangle inequality to cut a subtree
            //if (fabs(distanceRepres - currNode->GetEntry(idx).Distance) <=
              //  rangeK + currNode->GetRadius(idx)){
               // Rebuild the object
               tmpObj = currNode->GetObject(idx);
			   // is it a Representative?
               if (idx != idxRep) {
                  // No, it is not a representative. Evaluate distance
                  distance = tmpObj->GetMinDist( sample);
				  stRTree::NumberOfDC++;
               }else{
                  distance = distanceRepres;
               }//end if

               if (distance <= rangeK ){
                  // Yes! I'm qualified! Put it in the queue.
                  pqTmpValue.PageID = currNode->GetEntry(idx).PageID;
                  //pqTmpValue.Radius = currNode->GetRadius(idx);
                  // this is a subtree, put in the queue.
                  queue->Add(distance, pqTmpValue);
               }//end if
            //}//end if
         }//end if
      }//end for

      // If there is something in the queue.
      if (queue->GetSize() > 0) {

         this->NearestQuery(result, sample, rangeK, k, queue);
      }//end if

      // Release the Global Priority Queue.
      delete queue;
   }//end if
   return result;
    
}

void stRTree::NearestQuery(tResult * result,
         tObject * sample, stDistance rangeK, stCount k,
         tDynamicPriorityQueue * queue){

   stCount idx;
   tLogicNode * currNode;
   tObject* tmpObj;
   stDistance distance;
   stDistance distanceRepres = 0;
   stCount numberOfEntries;
   stQueryPriorityQueueValue pqCurrValue;
   stQueryPriorityQueueValue pqTmpValue;
   bool stop;

   queue->Get(distance, pqCurrValue);
   distanceRepres = distance;
   stDistance dOkQ;
  // tObject h;
   int kIndex = -1;

   // Let's search
   while (pqCurrValue.PageID != 0){
      // Read node...
      currNode = pqCurrValue.PageID;
      // Get the number of entries
      numberOfEntries = currNode->GetNumberOfEntries();
	  //Reset values for Lemma1
	  dOkQ = -1.0f;
	  kIndex = -1;
      // Is there any free objects in this node?
      if (currNode->GetNumberOfFreeObjects() > 0){

         // Now do it all free objects.
         for (idx = 0; idx < numberOfEntries; idx++) {
            // tests if this is a subtree.
            if (!currNode->GetEntry(idx).PageID){
               tmpObj = currNode->GetObject(idx);

		       distance = tmpObj->GetMinDist(sample);
		       stRTree::NumberOfDC++;
			   if (distance <= rangeK) {
		          // No, there is not a subtree. But this object qualifies.
		          // Add the object.
		          result->AddPair(tmpObj->Clone(), distance);
		          // there is more than k elements?
		          if (result->GetNumOfEntries() >= k){
		             //cut if there is more than k elements
		             result->Cut(k);
		             //may I use this for performance?
		             rangeK = result->GetMaximumDistance();
		          }//end if
		       }//end if
	       }//end if
         }//end for

      }//end if

      // Now do it for subtrees.
      for (idx = 0; idx < numberOfEntries; idx++) {
         // tests if this is a subtree.
         if (currNode->GetEntry(idx).PageID){
			   tmpObj = currNode->GetObject(idx);
  			   distance = tmpObj->GetMinDist(sample);
				stRTree::NumberOfDC++;
			   if (distance <= rangeK ){
				  pqTmpValue.PageID = currNode->GetEntry(idx).PageID;
				  queue->Add(distance, pqTmpValue);
			   }//end if
		}//end if
      }//end for

      // Go to next node
      stop = false;
      do{
         if (queue->Get(distance, pqCurrValue)){
            // Qualified if distance <= rangeK 
            if (distance <= rangeK ){
               // Yes, get the pageID and the distance from the representative
               // and the query object.
               distanceRepres = distance;
               // Break the while.
               stop = true;
            }//end if
         }else{
            // the queue is empty!
            pqCurrValue.PageID = 0;
            // Break the while.
            stop = true;
         }//end if
      }while (!stop);
   }// end while

}//end stDBMTree::NearestQuery

#endif	/* _stRTree_H */
