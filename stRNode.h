// 
// File:   stRNode.h
// Author: aocsa
//
// Created on January 17, 2007, 8:44 PM
//

#ifndef _stRNode_H
#define	_stRNode_H

#include <arboretum/stTypes.h>
#include "stMBR.h"

#include "stHyperCube.h"

//#define  PAGE_SIZE  (10 * 1024)	
//DIMENSION 54

#define  PAGE_SIZE  (256)	
//DIMENSION 3

#define NODE_HEAD_SIZE sizeof(stRNode::stRNodeHeader) 
#define ENTRY_SIZE sizeof(stRNode::stREntry)

class stRNode {
public:
    struct stREntry {
        stPageID PageID;
        //stSize   Offset;
		stSize	 NEntries;
    };
    struct stRNodeHeader {
        stCount     Occupation;
		stSize		ObjSize;
		stSize		Order;
    };
    
public:
    stRNode(stPage* page, int order = 0) {
		Page = page;
		Header = (stRNodeHeader*)(Page->GetData());
		Entries = (stREntry*)(Page->GetData() + sizeof(stRNodeHeader)); 
		if(order != 0) {
			Header->Occupation = 0;
			Header->Order = order;
			//Header->ObjSize = OBJECT_SIZE;
		}
	}    
    int AddEntry(int objectSize, const stByte* obj, stPageID subTree ) {
		Header->ObjSize = objectSize;
		memcpy(	this->Page->GetData() + GetFirstPosObject() + objectSize*Header->Occupation,
				obj,
				objectSize );

		Entries[Header->Occupation].PageID = subTree;
		++Header->Occupation;
		return Header->Occupation - 1;
	
	}
	void SetEntry(int idx, int objectSize, const stByte* obj, stPageID subTree ) {
		Header->ObjSize = objectSize;
		memcpy(	this->Page->GetData() + GetFirstPosObject() + objectSize*idx,
				obj,
				objectSize );

		Entries[idx].PageID = subTree;		
	}
	void RemoveEntry(int idx) {
		assert(idx < GetNumberOfEntries());
		// Update the usedSize
	   stCount i;
	   stCount lastID;
	   stSize rObjSize;

	   lastID = this->Header->Occupation - 1; 

	   if (lastID != (unsigned int)idx){
		  rObjSize = GetObjectSize(idx);

		  // Let's move objects first. We will use memmove() from stdlib because
		  // it handles the overlap between src and dst. Remember that src is the
		  // offset of the last object and the dst is the offset of the last
		  // object plus removed object size.
		  memmove(Page->GetData() + GetFirstPosObject() + Header->ObjSize * (idx),
				  Page->GetData() + GetFirstPosObject() + Header->ObjSize * (idx + 1),
				  Header->ObjSize * (lastID - idx) );
		  
		  // Let's move entries...
		  for (i = idx; i < lastID; i++){
			 // Copy all fields with memcpy (it's faster than field copy).
			 memcpy(Entries + i, Entries + i + 1, sizeof(stREntry));			 
		  }//end 
	   }//end if

	   // Update counter...
	   Header->Occupation--;
	}
    stByte* GetObject(int idx) {
		return this->Page->GetData() + GetFirstPosObject() + Header->ObjSize*idx;
	}
    stSize  GetObjectSize(int idx) {
		return Header->ObjSize;
	}
    stREntry& GetEntry(int idx) {
		assert(idx < this->Header->Occupation);
		return this->Entries[idx];
	}
	void SetNENtries(int idx, stCount NEntries) {
		GetEntry(idx).NEntries = NEntries;
	}
    stCount   GetNEntries(int idx) {
		return GetEntry(idx).NEntries;
	}
    void RemoveAll() {
		this->Header->Occupation = 0;
	}
    stCount GetNumberOfEntries() {
		return this->Header->Occupation;
	}
    stCount GetTotalObjectCount() {
		stCount sum = 0;
		for (int i = 0; i < GetNumberOfEntries(); i++){
			sum += GetNEntries(i);
		}
		return sum;
	}
	stPage* GetPage() {
		return Page;
	}
	stPageID GetPageID() {
		return this->Page->GetPageID();
	}
	bool	IsOverflow() const	{
		return Header->Order < Header->Occupation; 
	}

	int GetFirstPosObject () {
		return sizeof(stRNodeHeader) + (this->Header->Order + 1)*sizeof(stREntry);
	}

	stSize GetNumberOfFreeObjects() {
		stSize num = 0;
		for (int i = 0; i < GetNumberOfEntries(); i++) {
			if(Entries[i].PageID == 0){
				num++;
			}
		}
		return num;
	}

protected:
	stPage*		   Page;
	stRNodeHeader* Header;
	stREntry*	   Entries;
};


#endif	/* _stRNode_H */

