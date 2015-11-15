#include "stdafx.h"
#pragma warning(disable:4996)
#include <pcl/ModelCoefficients.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/passthrough.h>
#include <pcl/io/ply_io.h>
#include <pcl/filters/radius_outlier_removal.h>

typedef pcl::PointXYZRGBNormal PointT;

pcl::PointCloud<PointT>::Ptr
load(const char *fileName)
{
	pcl::PLYReader reader;
	pcl::PointCloud<PointT>::Ptr cloud(new pcl::PointCloud<PointT>);
	reader.read(fileName, *cloud);
	return cloud;
}

pcl::PointCloud<PointT>::Ptr
cylinderCrop(pcl::PointCloud<PointT>::Ptr cloud, double yc, double zc, double R)
{
	pcl::PointCloud<PointT>::Ptr cloud2(new pcl::PointCloud<PointT>);
	double R2 = R*R;
	for (size_t i = 0; i < cloud->size(); ++i) {
		PointT &pt = (*cloud)[i];
		double dy = pt.y - yc;
		double dz = pt.z - zc;
		double r2 = dy*dy + dz*dz;
		if (r2 < R2) {
			cloud2->push_back(pt);
		}
	}
	return cloud2;
}

pcl::PointCloud<PointT>::Ptr
radiusOutliersRemoval(pcl::PointCloud<PointT>::Ptr cloud, double r, int neighbours)
{
	pcl::PointCloud<PointT>::Ptr cloud2(new pcl::PointCloud<PointT>);
	pcl::RadiusOutlierRemoval<PointT> rorfilter;
	rorfilter.setInputCloud(cloud);
	rorfilter.setRadiusSearch(r);
	rorfilter.setMinNeighborsInRadius(neighbours);
	rorfilter.filter(*cloud2);
	return cloud2;
}

int
secs(clock_t t)
{
	return (int)((double)t / CLOCKS_PER_SEC);
}

int
main(int argc, char** argv)
{
	const char *fileName = NULL;
	double yc = 0.39577119586776877;
	double zc = 5.765876350413221;
	double R = 2.5;
	double r = 0.1;
	int neighbours = 5;
	int st = 0;
	for (int i = 1; i < argc; ++i)
	{
		const char *arg = *++argv;
		switch (st) {
		case 0:
			if (strcmp(arg, "--out-radius") == 0)
			{
				st = 1;
			}
			else if (strcmp(arg, "--out-neighbours") == 0)
			{
				st = 2;
			}
			else if (strcmp(arg, "--cyl-yc") == 0)
			{
				st = 3;
			}
			else if (strcmp(arg, "--cyl-zc") == 0)
			{
				st = 4;
			}
			else if (strcmp(arg, "--cyl-radius") == 0)
			{
				st = 5;
			}
			else
			{
				fileName = arg;
			}
			break;
		case 1:
			r = atof(arg);
			st = 0;
			break;
		case 2:
			neighbours = atoi(arg);
			st = 0;
			break;
		case 3:
			yc = atof(arg);
			st = 0;
			break;
		case 4:
			zc = atof(arg);
			st = 0;
			break;
		case 5:
			R = atof(arg);
			st = 0;
			break;
		}
	}
	clock_t t = clock();
	pcl::PointCloud<PointT>::Ptr cloud = load(fileName);
	t = clock() - t;
	std::cerr << secs(t) << " PointCloud has: " << cloud->size() << " data points." << std::endl;
	t = clock();
	cloud = cylinderCrop(cloud, yc, zc, R);
	t = clock() - t;
	std::cerr << secs(t) << " PointCloud has: " << cloud->size() << " data points." << std::endl;
	t = clock();
	cloud = radiusOutliersRemoval(cloud, r, neighbours);
	t = clock() - t;
	std::cerr << secs(t) << " PointCloud has: " << cloud->size() << " data points." << std::endl;
	t = clock();
	pcl::PLYWriter writer;
	writer.write("out.ply", *cloud, false, true);
	t = clock() - t;
	std::cerr << secs(t) << " PointCloud written to out.ply" << std::endl;
	return 0;
}