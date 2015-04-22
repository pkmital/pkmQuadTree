/*

 pkmQuadTree

 Parag K. Mital, April 21, 2015 
 
 Copyright Firef.ly Experience, Location Kit
 
 */

#include <memory>
#include <vector>

namespace pkm {
    
//--------------------------------------------------------------------------------------------------
#pragma mark QuadTreeBoundingBox
    
    //----------------------------------------------------------------------------------------------
    template <typename LocationType>
    class QuadTreeBoundingBox
    {
    public:
        //----------------------------------------------------------------------
        QuadTreeBoundingBox();
        QuadTreeBoundingBox(LocationType x_min, LocationType x_max, LocationType y_min, LocationType y_max);
        
        //----------------------------------------------------------------------
        void getMidpoint(LocationType &x, LocationType &y);
        
        //----------------------------------------------------------------------
        bool contains(LocationType x, LocationType y);
        
        //----------------------------------------------------------------------
        bool intersects(QuadTreeBoundingBox<LocationType> box);
        
    private:
        LocationType x_min, x_max, y_min, y_max;
    };
    
//--------------------------------------------------------------------------------------------------
#pragma mark QuadTreeNodeData
    
    //----------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    class QuadTreeNodeData
    {
    public:
        //----------------------------------------------------------------------
        QuadTreeNodeData(LocationType x, LocationType y, DataType data);
    private:
        LocationType x, y;
        DataType data;
    };
    
//--------------------------------------------------------------------------------------------------
#pragma mark QuadTreeNode

    //----------------------------------------------------------------------------------------------
    template <typename LocationType, class DataType>
    class QuadTreeNode
    {
    public:
        //----------------------------------------------------------------------
        QuadTreeNode();
        QuadTreeNode(QuadTreeBoundingBox<LocationType> boundary, int bucketCapacity);
        virtual ~QuadTreeNode();
        
        //----------------------------------------------------------------------
        void divide();
        
        //----------------------------------------------------------------------
        bool insert(const QuadTreeNodeData<LocationType, DataType> &data);
        
        //----------------------------------------------------------------------
        void getDataInBoundingBox(QuadTreeBoundingBox<LocationType> box, std::vector<QuadTreeNodeData<LocationType, DataType>> &data);
        
        //----------------------------------------------------------------------
        void performBlockOnDataInBoundingBox(QuadTreeBoundingBox<LocationType> box, void(^block)(const QuadTreeNodeData<LocationType, DataType> &data));
        void performBlockOnAllData(void(^block)(const QuadTreeNodeData<LocationType, DataType> &data));
        
        //----------------------------------------------------------------------
        inline int size();
        
        //----------------------------------------------------------------------
        static QuadTreeNode<LocationType, DataType> createQuadTreeNode(QuadTreeBoundingBox<LocationType> boundary,
                                  int bucketCapacity)
        {
            QuadTreeNode<LocationType, DataType> node(boundary, bucketCapacity);
            return node;
        }

    private:
        
        std::unique_ptr<QuadTreeNode> NW;
        std::unique_ptr<QuadTreeNode> NE;
        std::unique_ptr<QuadTreeNode> SW;
        std::unique_ptr<QuadTreeNode> SE;
        
        std::vector<QuadTreeNodeData<LocationType, DataType> > data;
        
        QuadTreeBoundingBox<LocationType> boundingBox;
        
        int bucketCapacity;
    };
    
    
//--------------------------------------------------------------------------------------------------
#pragma mark GPSQuadTree
    
    // Store a connection between lat/lon pairs for drawing
    template <typename LocationType>
    class Edge {
    public:
        LocationType x1, x2, y1, y2;
    };
    
    //----------------------------------------------------------------------------------------------
    template <typename LocationType = double, class DataType = Edge<LocationType> >
    class GPSQuadTree
    {
    public:
        //----------------------------------------------------------------------
        GPSQuadTree()
        {
            
        }
        
        //----------------------------------------------------------------------
        virtual ~GPSQuadTree()
        {
            root.reset();
        }
        
        //----------------------------------------------------------------------
        void insert(LocationType latitude, LocationType longitude, DataType data)
        {
            insert(QuadTreeNodeData<LocationType, DataType>(latitude, longitude, data));
        }
        
        //----------------------------------------------------------------------
        void insert(const QuadTreeNodeData<LocationType, DataType> &data)
        {
            root.insert(data);
        }
        
        void getDataInBoundingBox(QuadTreeBoundingBox<LocationType> boundary, std::vector<QuadTreeNodeData<LocationType, DataType>> &data)
        {
            root.getDataInBoundingBox(boundary, data);
        }
        
        //----------------------------------------------------------------------
        void allocateFromData(const std::vector<QuadTreeNodeData<LocationType, DataType> > &data,
                              QuadTreeBoundingBox<LocationType> boundary,
                              int bucketCapacity)
        {
            root = (new QuadTreeNode<LocationType, DataType>(boundary, bucketCapacity));
            for (int i = 0; i < data.size(); i++) {
                root->insert(data[i]);
            }
        }
        
    private:
        std::unique_ptr<QuadTreeNode<LocationType, DataType> > root;
    };
    
}




