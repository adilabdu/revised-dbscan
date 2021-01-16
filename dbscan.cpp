#include <iostream>
#include "dbscan.h"

int DBSCAN::run() {
    int clusterID = 1;
    vector<Point>::iterator iter;
    for (iter = m_points.end(); iter != m_points.begin(); --iter) {
        if (iter->clusterID == UNCLASSIFIED) {
            if (expandCluster(*iter, clusterID) != FAILURE) {
                clusterID += 1;
            }
        }
    }

    vector<Point>::iterator i;
    float closest = m_epsilon;
    for(i = m_points.begin(); i != m_points.end(); ++i) {
        if(i->pointType == BORDER_POINT) {
            vector<int> borderNeighbors = calculateCluster(*i);
            for (std::vector<int>::iterator j = borderNeighbors.begin() ; j != borderNeighbors.end(); ++j) {
                if(m_points.at(*j).pointType == CORE_POINT) {
                
                    if(calculateDistance(m_points.at(*j), *i) < closest) {
                        
                        closest = calculateDistance(m_points.at(*j), *i);
                        i->clusterID = m_points.at(*j).clusterID;
	            	// i->clusterID = 10;        
		    }
                
                }
            }            
        }
        closest = m_epsilon;
    }

    return 0;
}

int DBSCAN::expandCluster(Point point, int clusterID) {
    vector<int> clusterSeeds = calculateCluster(point);

    if (clusterSeeds.size() < m_minPoints) {
        point.clusterID = NOISE;
        point.pointType = NOISE;
        return FAILURE;
    } else {
        int index = 0, indexCorePoint = 0;
        point.clusterID = clusterID;
        point.pointType = CORE_POINT; // set this point to CORE POINT
        vector<int>::iterator iterSeeds;
        for (iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds) {
            // m_points.at(*iterSeeds).clusterID = clusterID;
            if (m_points.at(*iterSeeds).x == point.x && m_points.at(*iterSeeds).y == point.y &&
                m_points.at(*iterSeeds).z == point.z) {
                indexCorePoint = index;
            }
            ++index;
        }
        clusterSeeds.erase(clusterSeeds.begin() + indexCorePoint);

        for( vector<int>::size_type i = 0, n = clusterSeeds.size(); i < n; ++i )
        {
            vector<int> clusterNeighors = calculateCluster(m_points.at(clusterSeeds[i]));

            if ( clusterNeighors.size() >= m_minPoints )
            {
                m_points.at(clusterSeeds[i]).clusterID = clusterID;
                m_points.at(clusterSeeds[i]).pointType = CORE_POINT; // CORE_POINT to element with neighbors over minPoints
                vector<int>::iterator iterNeighors;
                for ( iterNeighors = clusterNeighors.begin(); iterNeighors != clusterNeighors.end(); ++iterNeighors )
                {
                    if ( m_points.at(*iterNeighors).clusterID == UNCLASSIFIED || m_points.at(*iterNeighors).clusterID == NOISE )
                    {
                        if ( m_points.at(*iterNeighors).clusterID == UNCLASSIFIED )
                        {
                            clusterSeeds.push_back(*iterNeighors);
                            n = clusterSeeds.size();
                        }
                        // m_points.at(*iterNeighors).clusterID = clusterID;
                        m_points.at(*iterNeighors).pointType = BORDER_POINT;
                        m_points.at(*iterNeighors).clusterID = NOISE;
                    }
                }
            }
        }

        return SUCCESS;
    }
}

vector<int> DBSCAN::calculateCluster(Point point)
{
    int index = 0;
    vector<Point>::iterator iter;
    vector<int> clusterIndex;
    for( iter = m_points.begin(); iter != m_points.end(); ++iter)
    {
        if ( calculateDistance(point, *iter) <= m_epsilon )
        {
            clusterIndex.push_back(index);
        }
        index++;
    }
    return clusterIndex;
}

inline double DBSCAN::calculateDistance(Point pointCore, Point pointTarget) {
    return pow(pointCore.x - pointTarget.x, 2) + pow(pointCore.y - pointTarget.y, 2) /** +
           pow(pointCore.z - pointTarget.z, 2)**/;
}
