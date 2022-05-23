#ifndef __PHOTONMAP_HPP__
#define __PHOTONMAP_HPP__

#include "scene/Camera.hpp"
#include "geometry/vec.hpp"
#include <queue>

#include "Ray.hpp"
using namespace NRenderer;
using namespace std;

struct Photon
{
	Vec3 position;
	Vec3 direction;
	Vec3 power;
	float dis;
	int diffAxis;
	Photon() {
		diffAxis = 0;
	}
};
class KnnPhoton {
public:
	Vec3 posi;
	int found;
	double dis;
	struct cmp
	{
		bool operator()(const Photon& a, const Photon& b)
		{
			return a.dis < b.dis;
		}
	};
	priority_queue<Photon, vector<Photon>, cmp> maxHeap;

};


class PhotonMap {
public:
	PhotonMap();
	PhotonMap(int max);
	~PhotonMap();
	int number;
	int maxNumber;
	Photon* tPhoton;
	void storePhoton(Photon p);
	Vec3 boxMax;
	Vec3 boxMin;
	Vec3 random_vec(int seed);
	void generPhoton(Vec3& dir, int seed);
	int getMiddle(int s, int e);
	void sortMiddle(Photon* p, int s, int e, int m, int axis);
	void generKdtree();
	void balance(Photon* p, int i, int s, int e);
	void getNPhoton(KnnPhoton* np, int index);
	void getNPhoton(KnnPhoton* np);
};

#endif