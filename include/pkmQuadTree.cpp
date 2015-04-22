/*
 
 pkmQuadTree
 
 Written by Parag K. Mital, April 21, 2015
 Copyright Firef.ly Experience, Location Kit
 
 */

#include "pkmQuadTree.h"

namespace pkm {
    
//--------------------------------------------------------------------------------------------------
#pragma mark QuadTreeBoundingBox
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType>
    QuadTreeBoundingBox<LocationType>::QuadTreeBoundingBox()
    {
        x_min = x_max = y_min = y_max = 0;
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType>
    QuadTreeBoundingBox<LocationType>::QuadTreeBoundingBox(LocationType x_min,
                        LocationType x_max,
                        LocationType y_min,
                        LocationType y_max)
    {
        this->x_min = x_min;
        this->x_max = x_max;
        this->y_min = y_min;
        this->y_max = y_max;
    }

    //--------------------------------------------------------------------------------------------------
    template <typename LocationType>
    void QuadTreeBoundingBox<LocationType>::getMidpoint(LocationType &x, LocationType &y)
    {
        x = (x_min + x_max) / 2.0;
        y = (y_min + y_max) / 2.0;
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType>
    bool QuadTreeBoundingBox<LocationType>::contains(LocationType x, LocationType y)
    {
        return (x_min <= x && x <= x_max && y_min <= y && y <= y_max);
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType>
    bool QuadTreeBoundingBox<LocationType>::intersects(QuadTreeBoundingBox<LocationType> box)
    {
        return (x_min <= box.x_max && x_max >= box.x_min && y_min <= box.y_max && y_max >= box.y_min);
    }
    
//--------------------------------------------------------------------------------------------------
#pragma mark QuadTreeNodeData
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    QuadTreeNodeData<LocationType, DataType>::QuadTreeNodeData(LocationType x, LocationType y, DataType data)
    {
        this->x = x;
        this->y = y;
        this->data = data;
    }
    
//--------------------------------------------------------------------------------------------------
#pragma mark QuadTreeNode
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    QuadTreeNode<LocationType, DataType>::QuadTreeNode()
    {
        NW = nullptr;
        NE = nullptr;
        SW = nullptr;
        SE = nullptr;
        
        bucketCapacity = 0;
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    QuadTreeNode<LocationType, DataType>::QuadTreeNode(QuadTreeBoundingBox<LocationType> boundary,
                 int bucketCapacity)
    {
        NW = nullptr;
        NE = nullptr;
        SW = nullptr;
        SE = nullptr;
        
        this->boundingBox = boundary;
        this->bucketCapacity = bucketCapacity;
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    QuadTreeNode<LocationType, DataType>::~QuadTreeNode()
    {
        NW.reset();
        NE.reset();
        SE.reset();
        SW.reset();
        
        data.reset();
        
        bucketCapacity = 0;
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    void QuadTreeNode<LocationType, DataType>::divide()
    {
        LocationType x_mid, y_mid;
        boundingBox.getMidpoint(x_mid, y_mid);
        
        NW = (new QuadTreeNode(QuadTreeBoundingBox<LocationType>(boundingBox.x_min, boundingBox.y_min, x_mid, y_mid), bucketCapacity));
        NE = (new QuadTreeNode(QuadTreeBoundingBox<LocationType>(x_mid, boundingBox.y_min, boundingBox.x_max, y_mid), bucketCapacity));
        SW = (new QuadTreeNode(QuadTreeBoundingBox<LocationType>(boundingBox.x_min, y_mid, x_mid, boundingBox.y_max), bucketCapacity));
        SE = (new QuadTreeNode(QuadTreeBoundingBox<LocationType>(x_mid, y_mid, boundingBox.x_max, boundingBox.y_max), bucketCapacity));
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    bool QuadTreeNode<LocationType, DataType>::insert(const QuadTreeNodeData<LocationType, DataType> &data)
    {
        if (!boundingBox.contains(data.x, data.y)) {
            return false;
        }
        
        if (size() < bucketCapacity) {
            this->data.push_back(data);
            return true;
        }
        
        if (NW == nullptr) {
            divide();
        }
        
        if (NW->insert(data)) return true;
        if (NE->insert(data)) return true;
        if (SW->insert(data)) return true;
        if (SE->insert(data)) return true;
        
        return false;
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    void QuadTreeNode<LocationType, DataType>::getDataInBoundingBox(QuadTreeBoundingBox<LocationType> box, std::vector<QuadTreeNodeData<LocationType, DataType>> &data)
    {
        if (!this->box.intersects(box)) {
            return;
        }
        
        for (int i = 0; i < size(); i++) {
            if (box.contains(this->data[i].x, this->data[i].y)) {
                data.push_back(this->data[i]);
            }
        }
        
        if (NW == nullptr) {
            return;
        }
        
        getDataInBoundingBox(NW, data);
        getDataInBoundingBox(NE, data);
        getDataInBoundingBox(SW, data);
        getDataInBoundingBox(SE, data);
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    void QuadTreeNode<LocationType, DataType>::performBlockOnDataInBoundingBox(QuadTreeBoundingBox<LocationType> box, void(^block)(const QuadTreeNodeData<LocationType, DataType> &data))
    {
        if (!this->box.intersects(box)) {
            return;
        }
        
        for (int i = 0; i < size(); i++) {
            if (box.contains(data[i].x, data[i].y)) {
                block(data[i]);
            }
        }
        
        if (NW == nullptr) {
            return;
        }
        
        performBlockOnDataInBoundingBox(NW, data, block);
        performBlockOnDataInBoundingBox(NE, data, block);
        performBlockOnDataInBoundingBox(SW, data, block);
        performBlockOnDataInBoundingBox(SE, data, block);
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    void QuadTreeNode<LocationType, DataType>::performBlockOnAllData(void(^block)(const QuadTreeNodeData<LocationType, DataType> &data))
    {
        performBlockOnDataInBoundingBox(boundingBox, block);
    }
    
    //--------------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    inline int QuadTreeNode<LocationType, DataType>::size()
    {
        return data.size();
    }

//--------------------------------------------------------------------------------------------------
#pragma mark QuadTree
}