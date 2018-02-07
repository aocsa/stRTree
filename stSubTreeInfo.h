// 
// File:   stSubTreeInfo.h
// Author: aocsa
//
// Created on January 23, 2007, 10:50 AM
//

#ifndef _stSubTreeInfo_H
#define	_stSubTreeInfo_H


template<class ObjectType> 
class stRLogicNode;

template<class ObjectType> 
struct stSubTreeInfo {
    ObjectType*					Rep;
    stRLogicNode<ObjectType>*   RootID;
    stCount						NObjects;
    stSize						Height;
};
    

#endif	/* _stSubTreeInfo_H */

