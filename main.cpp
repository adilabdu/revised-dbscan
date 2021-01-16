#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include "dbscan.h"

using namespace std::chrono;

#define MINIMUM_POINTS 50 // minimum number of cluster
// #define EPSILON (0.75*0.75)  // distance for clustering, metre^2

// #define EPSILON (0.499056 * 0.499056)
#define EPSILON (0.4 * 0.4)

void readBenchmarkData(vector<Point> &points)
{
    // load point cloud
    FILE *stream;
    stream = fopen("datasets/TwoDiamonds.data", "ra");

    unsigned int minpts, num_points, cluster, i = 0;
    double epsilon;
    fscanf(stream, "%u\n", &num_points);

    Point *p = (Point *)calloc(num_points, sizeof(Point));

    while (i < num_points)
    {
        fscanf(stream, "%f,%f,%d\n", &(p[i].x), &(p[i].y), &cluster);
        p[i].clusterID = UNCLASSIFIED;
        points.push_back(p[i]);
        ++i;
    }

    free(p);
    fclose(stream);
}

void printResults(vector<Point> &points, int num_points)
{
    int i = 0;
    int outliers = 0;
    printf("Number of points: %u\n"
           " x     y     z     cluster_id\n"
           "-----------------------------\n",
           num_points);
    while (i < num_points)
    {
        printf("%5.2lf %5.2lf %5.2lf: %d\n",
               points[i].x,
               points[i].y, points[i].z,
               points[i].clusterID);
        ++i;
    }
}

void writeResults(vector<Point> &points, int num_points)
{
    int i = 0;
    int outliers = 0;
    int clusters = 0;
    std::ofstream Clusters("csv/twodiamonds-revised-dbscan.csv");
    Clusters << "x,y,Cluster" << std::endl;
    while (i < num_points)
    {
        Clusters << points[i].x << "," << points[i].y << ","
                 << points[i].clusterID << std::endl;

	if(points[i].clusterID == -1)
	{
	    outliers++;
	}

	if(points[i].clusterID > clusters)
	{
	    clusters = points[i].clusterID;
	}

	i++;
    }

    Clusters.close();
    cout << "Detected Clusters: " << clusters << endl;
    cout << "Outliers: " << outliers << endl;
}

int main(int argc, char** argv)
{
    // Get starting timepoint
    auto start = high_resolution_clock::now();

    vector<Point> points;

    // read point data
    readBenchmarkData(points);

    // constructor
    DBSCAN ds(MINIMUM_POINTS, EPSILON, points);

    // main loop
    ds.run();

    // result of DBSCAN algorithm
    writeResults(ds.m_points, ds.getTotalPointSize());

    // Get ending timepoint
    auto stop = high_resolution_clock::now();

    microseconds duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() * 0.000001 << " seconds" << endl;

    return 0;
}
