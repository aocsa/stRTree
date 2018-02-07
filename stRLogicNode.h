// stRLogicNode.h: interface for the stRLogicNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRLOGICNODE_H__A740EE29_0F36_4F34_83F7_9EE82A938BA1__INCLUDED_)
#define AFX_STRLOGICNODE_H__A740EE29_0F36_4F34_83F7_9EE82A938BA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "stRTree.h"

#include <arboretum/stTypes.h>
#include <arboretum/stUtil.h>
#include <arboretum/stCommon.h>
#include "stSubTreeInfo.h"

//#include "stRNode.h"

#define  PAGE_SIZE  (256)	

template<class tObject>
class stRLogicNode  
{
    typedef stSubTreeInfo<tObject> tSubTreeInfo;
    struct stLogicEntry;
    
public:
	stRLogicNode(stSize n);
	virtual ~stRLogicNode();
	void SetMinOccupation(int occup);
	int  AddEntry(tObject* obj, stSize NEntries, stRLogicNode* pageID);
	void SetEntry(int idx, tObject* obj, stSize NEntries, stRLogicNode* pageID);
	//void AddNode(stRNode* node);
	void AddNode(stRLogicNode* node);
	tObject* GetObject(int idx) {
		return Entries[idx].Object;
	}
	void Distribute(stRLogicNode* node0, stRLogicNode* node1, tSubTreeInfo* promo);
	stSize GetNumberOfEntries() {
		return this->Occupation;
	}
	bool IsOverflow() {
		return Occupation > MaxNumberOfEntries;
	}
	void RemoveAll();

	stCount GetNEntries(int idx) {
		return Entries[idx].NEntries;
	}
	stLogicEntry& GetEntry(int idx) {
		return Entries[idx];
	}
	stRLogicNode* GetPageID() {
		return this;
	}
	stCount GetNumberOfFreeObjects() {
		stSize num = 0;
		for (int i = 0; i < GetNumberOfEntries(); i++) {
			if(Entries[i].PageID == 0){
				num++;
			}
		}
		return num;
	}
	stCount GetTotalObjectCount() {
		stCount sum = 0;
		for (int i = 0; i < GetNumberOfEntries(); i++){
			sum += GetNEntries(i);
		}
		return sum;
	}
	
	void RemoveEntry(int idx);
	stMBR*	GetFirstMBR(stRLogicNode* currNode) ;
	void SetParent(stRLogicNode* p) {
		Parent = p;
	}
	stRLogicNode* GetParent() {
		return Parent;
	}
	bool IsLeaf() {
		return Entries[0].PageID == NULL;
	}
private:

	int		SelecPickNext(stMBR*  mbr0, stMBR*  mbr1);
	void	SelectPickSeeds(int& pos0, int& pos1);

private:
	struct stLogicEntry {
		tObject*		Object;
		stSize			NEntries;
		stRLogicNode*	PageID;		
		bool			Mine;
	};
	stRLogicNode* Parent;	
	stLogicEntry* Entries;
	stCount		  Occupation;
	stSize		  MaxNumberOfEntries;
	float		  MinOccupation;
};	


template<class tObject>
stRLogicNode<tObject>::stRLogicNode(stSize n){

	Parent = NULL;
	MaxNumberOfEntries = n;
	Entries = new stLogicEntry[MaxNumberOfEntries + 1];
	Occupation = 0;
}

template<class tObject>
stRLogicNode<tObject>::~stRLogicNode()
{
	int idx;
	for (idx = 0; idx < this->GetNumberOfEntries(); idx++) {
		if( Entries[idx].Mine )
			delete Entries[idx].Object;
	}
	delete [] Entries;
}

template<class tObject>
void stRLogicNode<tObject>::SetMinOccupation(int occup)
{
	MinOccupation = occup;
}

/*template<class tObject>
void stRLogicNode<tObject>::AddNode(stRNode* node)
{
	int idx;
	for (idx = 0; idx < node->GetNumberOfEntries(); idx++) {
		tObject* tmpObj = new tObject;
		tmpObj->Unserialize(node->GetObject(idx), node->GetObjectSize(idx));
		this->AddEntry(tmpObj, node->GetNEntries(idx), node->GetEntry(idx).PageID);
	}
}*/

template<class tObject>
void stRLogicNode<tObject>::RemoveAll()
{
	int idx;
	for (idx = 0; idx < this->GetNumberOfEntries(); idx++) {
		Entries[idx].Object = NULL;
	}
	Occupation = 0;
}

template<class tObject>
void stRLogicNode<tObject>::AddNode(stRLogicNode* node)
{
	int idx;
	for (idx = 0; idx < node->GetNumberOfEntries(); idx++) {
		tObject* tmpObj = node->GetObject(idx);
		this->AddEntry(tmpObj, node->GetNEntries(idx), node->GetEntry(idx).PageID);//??
	}
}

	
template<class tObject>
int  stRLogicNode<tObject>::AddEntry(tObject* obj, stSize NEntries, stRLogicNode* pageID)
{
	this->Entries[Occupation].Mine = true;
	this->Entries[Occupation].Object = obj;
	this->Entries[Occupation].PageID = pageID;
	this->Entries[Occupation].NEntries = NEntries;
	this->Occupation++;
	return Occupation - 1;
}

template<class tObject>
void stRLogicNode<tObject>::SetEntry(int idx, tObject* obj, stSize NEntries, stRLogicNode* pageID)
{
	this->Entries[idx].Mine = true;
	if(this->Entries[idx].Object)
		delete this->Entries[idx].Object;
	this->Entries[idx].Object = obj;
	this->Entries[idx].PageID = pageID;
	this->Entries[idx].NEntries = NEntries;
}

template<class tObject>
void stRLogicNode<tObject>::RemoveEntry(int idx)
{
	int lastID = this->GetNumberOfEntries() - 1;
	if(lastID != idx) {
		while (idx < lastID ) {
			Entries[idx].Mine = Entries[idx + 1].Mine;
			Entries[idx].NEntries = Entries[idx + 1].NEntries;
			Entries[idx].Object = Entries[idx + 1].Object;
			Entries[idx].PageID = Entries[idx + 1].PageID;
			idx++;
		}
	}
	this->Occupation--;
}
template<class tObject>
void stRLogicNode<tObject>::Distribute(stRLogicNode* nodeLogic0, stRLogicNode* nodeLogic1, tSubTreeInfo* promo)
{
	int idx, i, idxSelect, insertIdx, pos0, pos1;
	int numberOfEntriesAdded;
	stDistance IR0, IR1;
	numberOfEntriesAdded = 0;
	assert(nodeLogic0->GetNumberOfEntries() == 0);
	assert(nodeLogic1->GetNumberOfEntries() == 0);
#ifdef __stDEBUG__ 	 
	for (idx = 0; idx < this->GetNumberOfEntries(); idx++) {
		Entries[idx].Object->Print(std::cout);
	}
#endif

	pos0 = pos1 = -1;
	this->SelectPickSeeds(pos0, pos1);
	//std::cout << "pos0: " << pos0 << "\t" << "pos1: " << pos1 << "\n";
	this->Entries[pos0].Mine = false;
	this->Entries[pos1].Mine = false;
	
	nodeLogic0->AddEntry(this->Entries[pos0].Object, this->Entries[pos0].NEntries, this->Entries[pos0].PageID);
	if(this->IsLeaf())
		((stPoint*)this->Entries[pos0].Object)->SetNode(nodeLogic0);

	nodeLogic1->AddEntry(this->Entries[pos1].Object, this->Entries[pos1].NEntries, this->Entries[pos1].PageID);
	if(this->IsLeaf())
		((stPoint*)this->Entries[pos1].Object)->SetNode(nodeLogic1);

	numberOfEntriesAdded = 2;
	while(numberOfEntriesAdded != this->GetNumberOfEntries()) {
		
		// find mbr of each group.
		stMBR* mbr0 = GetFirstMBR(nodeLogic0);	//nodeLogic0->Entries[0].Object;

		for (i = 1; i < nodeLogic0->GetNumberOfEntries(); i++) {
		    mbr0 = mbr0->GetUnionMBR(nodeLogic0->Entries[i].Object);
		}
		stMBR* mbr1 = GetFirstMBR(nodeLogic1);	//	->Entries[0].Object;
		for (i = 1; i < nodeLogic1->GetNumberOfEntries(); i++) {
		    mbr1 = mbr1->GetUnionMBR(nodeLogic1->Entries[i].Object);
		}
		idxSelect = this->SelecPickNext(mbr0, mbr1);
	 
#ifdef __stDEBUG__
		std::cout << "idxSelect: " << idxSelect << "\n";
#endif
		stMBR* a = mbr0->GetUnionMBR( Entries[idxSelect].Object );
		IR0 = a->GetArea() - mbr0->GetArea();
		stMBR* b = mbr1->GetUnionMBR( Entries[idxSelect].Object );
		IR1 = b->GetArea() - mbr1->GetArea();
		
		if( IR0 < IR1 ) {
			nodeLogic0->AddEntry(this->Entries[idxSelect].Object, this->Entries[idxSelect].NEntries, this->Entries[idxSelect].PageID);
			if(this->IsLeaf())
				((stPoint*)this->Entries[idxSelect].Object)->SetNode(nodeLogic0);
		}
		else {
			nodeLogic1->AddEntry(this->Entries[idxSelect].Object, this->Entries[idxSelect].NEntries, this->Entries[idxSelect].PageID);
			if(this->IsLeaf())
				((stPoint*)this->Entries[idxSelect].Object)->SetNode(nodeLogic1);
		}
		numberOfEntriesAdded++;
		Entries[idxSelect].Mine = false;
		//////////////////////////////////////////////////////////////////////////
		delete mbr0;
		delete mbr1;
		delete a;
		delete b;
	}
	
	stMBR* mbr0 = GetFirstMBR(nodeLogic0);// stMBR*)nodeLogic0->Entries[0].Object->Clone();
	for (i = 1; i < nodeLogic0->GetNumberOfEntries(); i++) {
		mbr0 = mbr0->GetUnionMBR(nodeLogic0->Entries[i].Object);
	}
	stMBR* mbr1 = GetFirstMBR(nodeLogic1); //stMBR*)nodeLogic1->Entries[0].Object->Clone();
	for (i = 1; i < nodeLogic1->GetNumberOfEntries(); i++) {
		mbr1 = mbr1->GetUnionMBR(nodeLogic1->Entries[i].Object);
	}
	promo[0].RootID = nodeLogic0->GetPageID();
	promo[0].Rep = (tObject*)mbr0;
	promo[0].NObjects = nodeLogic0->GetTotalObjectCount();

	promo[1].RootID = nodeLogic1->GetPageID();
	promo[1].Rep = (tObject*)mbr1;
	promo[1].NObjects = nodeLogic1->GetTotalObjectCount();
}



template<class tObject>
void stRLogicNode<tObject>::SelectPickSeeds(int& pos0, int& pos1)
{
	stDistance  d, 
				max = -1.00;
	int i, j;
	for(i = 1; i < this->GetNumberOfEntries() ;i++ ) {
		for (j = 0; j < i; j++) {
			stMBR* mbr = Entries[i].Object->GetUnionMBR(Entries[j].Object);
			d = fabs(mbr->GetArea() - Entries[i].Object->GetArea() - Entries[j].Object->GetArea());
			if( d > max ){
				max = d;
				pos0 = i;
				pos1 = j;
			}
		}
	}
	assert(pos0 >= 0 && pos1 >= 0);
}

template<class tObject>
int stRLogicNode<tObject>::SelecPickNext(stMBR*  mbr0, stMBR*  mbr1)
{
	stDistance d0, d1, max = -1.00;
	int idx, ret = -1;
	for (idx = 0; idx < this->GetNumberOfEntries(); idx++){
		if(Entries[idx].Mine == false ) continue;

		stMBR* a = mbr0->GetUnionMBR( Entries[idx].Object );
		d0 = a->GetArea() - mbr0->GetArea();
		
		stMBR* b = mbr0->GetUnionMBR( Entries[idx].Object );
		d1 = b->GetArea() - mbr1->GetArea();
		
		if( fabs(d0 - d1) > max) {
			max = fabs(d0 - d1);
			ret = idx;
		}
		delete a;
		delete b;
	}
	
	//if (mask[i] != -1) {
	//HyperCube a = mbr1.getUnionMbb(data[i]);
	//d1 = a.getArea() - mbr1.getArea();
	assert(ret >= 0);		
	return ret;
}

template<class tObject>
stMBR*	stRLogicNode<tObject>::GetFirstMBR(stRLogicNode* currNode) 
{
	int idx;
	stHyperCube* mbr = new stHyperCube();
	for (idx = 0; idx <  currNode->GetObject(0)->GetDimension() ; idx++) {
		mbr->AddInterval(idx, currNode->GetObject(0)->GetBegin(idx), currNode->GetObject(0)->GetEnd(idx));
	}
	return mbr;
}

#endif // !defined(AFX_STRLOGICNODE_H__A740EE29_0F36_4F34_83F7_9EE82A938BA1__INCLUDED_)
