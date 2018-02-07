

//#define  __stDEBUG__ 1

#include <iostream>
#include <cstdlib>

#include <arboretum/stDiskPageManager.h>
#include "stHyperCube.h"
#include "stRTree.h"


int stRTree::NumberOfDC = 0;

int main()
{
	stHyperCube* cube0 = new stHyperCube;
	stHyperCube* cube1 = new stHyperCube;

	//for (idx = 0; idx < DIMENSION ; idx++) {
	cube0->AddInterval(0, 0, 10);
	cube0->AddInterval(1, 10, 20);
	
	cube1->AddInterval(0, 5, 15);
	cube1->AddInterval(1, 5, 10);
	
	if(cube0->Intersection(cube1)) 
		std::cout << "Intersection0\n";

	if(cube1->Intersection(cube0)) 
		std::cout << "Intersection1\n";

	stHyperCube* mbr3 = new stHyperCube(*(stHyperCube*)cube0->GetUnionMBR(cube1));

	stHyperCube* c = new stHyperCube;
	c->Unserialize( cube0->Serialize(), cube0->GetSerializedSize());
	c->Print(std::cout);

	cube0->Print(std::cout);
	cube1->Print(std::cout);
	
	stPoint p;
	p[0] = 0;
	p[1] = 0;
	//2] = 0;
	std::cout << "dist(p, cube0) = " << cube0->GetMinDist(&p) << "\n";

	for (int i = 0; i < DIMENSION;i++) {
		std::cout << mbr3->GetBegin(i) << " - " << mbr3->GetEnd(i) << "\n";
	}
	std::cout << "\n";

	stMBR* mbr = (stHyperCube*)mbr3->Clone();
	
	//stMBR* mbr = mbr3;
	std::cout << mbr->GetArea() << "\n";
	stHyperCube* mbrTmp = (stHyperCube*)mbr;
	mbrTmp->Serialize();
	std::cout << mbrTmp->GetArea() << "\n";
	
	stDiskPageManager* pageManager = new stDiskPageManager();
	pageManager->Create("index", PAGE_SIZE);

	stRTree* rtree = new stRTree(4);
	typedef std::pair<stDistance, stDistance>  Elem;
	std::vector<Elem > v;
	
	v.push_back(Elem(2,3));
	v.push_back(Elem(2,5));
	v.push_back(Elem(6,4));
	v.push_back(Elem(8,2));
	v.push_back(Elem(9,4));
	v.push_back(Elem(7,7));
	
	v.push_back(Elem(1,4));
	v.push_back(Elem(7,1));
	v.push_back(Elem(7,6));
	v.push_back(Elem(6,8));
	v.push_back(Elem(9,7));
	v.push_back(Elem(9,8));
	v.push_back(Elem(9,6));

	std::vector<stPoint*> points;
	for (i = 0; i < v.size(); i++) {
		stPoint* obj = new stPoint;
		obj->AddInterval(0, v[i].first);
		obj->AddInterval(1, v[i].second);
		obj->SetOID(i+1);
		rtree->Add(obj);
		
		points.push_back(obj);

	//	rtree->Print(std::cout);
	//	std::cin.get();	

	}
	rtree->Print(std::cout);
	std::cin.get();	


	int idx;
	for (i=0; i < points.size(); i++) {
		stRTree::tLogicNode* node = points[i]->GetNode();
		for (idx = 0; idx < node->GetNumberOfEntries(); idx++) {
			if(node->GetObject(idx)->IsEqual(points[i])) {
				std::cout << "found: " << i << "\n";
				break;
			}
		}
	}
	for (i = 0; i < points.size(); i++) {
		std::cout << "rm: " << *points[i] << "\n";
		rtree->Remove(points[i]->GetNode(), points[i]);		
		rtree->Print(std::cout);
		std::cin.get();	
	}
	
	stHyperCube* oldValue = new stHyperCube;
	oldValue->AddInterval(0, v[0].first);
	oldValue->AddInterval(1, v[0].second);
	oldValue->SetOID(1);

	stHyperCube* newValue = new stHyperCube;
	newValue->AddInterval(0, 1);
	newValue->AddInterval(1, 1);
	newValue->SetOID(1);

	oldValue->Print(std::cout);
	newValue->Print(std::cout);
	
	/*for (i = 0; i < 100; i++) {
		if ( rtree->UpdateData(oldValue, newValue) ) {
			//rtree->Print(std::cout);
			std::cout << "*** " << i << ", ";
		}
		else {
			oldValue->Print(std::cout);
			newValue->Print(std::cout);
			std::cout << "fallo el reemplazo\n";
			rtree->Print(std::cout);
		std::cin.get();

		}

		delete oldValue;
		oldValue = newValue->Clone();
		newValue->GetEnd(0) += 0.1; 
	}
	rtree->Print(std::cout);
	std::cin.get();*/

	stHyperCube* query = new stHyperCube;
	query->AddInterval(0, 6);
	query->AddInterval(1, 2);
	
	stRTree::tResult* result = rtree->NearestQuery(query, 3);
	for (i = 0; i < result->GetNumOfEntries(); i++) {
		((stHyperCube*)(*result)[i].GetObject())->Print(std::cout);
	}

	delete rtree;
	delete pageManager;
	return 0;
}