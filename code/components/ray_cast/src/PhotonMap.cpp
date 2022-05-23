#include "PhotonMap.hpp"
#include <random>

#include "intersections/intersections.hpp"
Vec3 PhotonMap::random_vec(int seed) {
	static default_random_engine e;
	static uniform_real_distribution<float> u(-1, 1);
	e.seed(time(0) + seed);
	Vec3 v;
	float mode;
	do
	{
		v = Vec3(u(e), u(e), u(e));
		mode = glm::dot(v, v);
	} while (mode == 0.0 || mode >= 1.0);
	return v;
}


void PhotonMap::generPhoton(Vec3& dir, int seed) {
	dir = random_vec(seed);

}


PhotonMap::PhotonMap() {
	maxNumber = 10000;
	number = 0;
	tPhoton = new Photon[10000+1];
	boxMax = Vec3(1000000.0, 1000000.0, 1000000.0);
	boxMin = Vec3(-1000000.0, -1000000.0, -1000000.0);

}


PhotonMap::PhotonMap(int max) {
	maxNumber = max;
	number = 0;
	tPhoton = new Photon[max+1];
	boxMax = Vec3(1000000.0, 1000000.0, 1000000.0);
	boxMin = Vec3(-1000000.0, -1000000.0, -1000000.0);

}


PhotonMap::~PhotonMap() {
	delete[] tPhoton;
}


void PhotonMap::storePhoton(Photon p) {
	if (number > maxNumber)
		return;
	number++;
	tPhoton[number] = p;
	boxMin = Vec3(min(boxMin.x, p.position.x), min(boxMin.y, p.position.y), min(boxMin.z, p.position.z));
	boxMax = Vec3(max(boxMin.x, p.position.x), max(boxMin.y, p.position.y), max(boxMin.z, p.position.z));
}


int PhotonMap::getMiddle(int s, int e) {
	int n = e - s + 1;
	int middle;
	int layerN = 2;
	int i = 1;
	while (i < n) {
		i += layerN;
		layerN *= 2;
	}
	if (i == n) {
		return s + n / 2;
	}
	layerN /= 2;
	if (i - layerN / 2 < n) {
		return s + i / 2;
	}
	else {
		return s + i / 2 - (i - layerN / 2 - n);
	}
}

//void PhotonMap::tracePhoton(RayCast::Ray& r, Vec3 power, int depth = 0) {
//
//
//}

void PhotonMap::sortMiddle(Photon* p, int s, int e, int m, int axis) {
	int left = s;
	int right = e;
	Photon temp;
	while (left < right) {
		double key = p[right].position[axis];
		int i = left - 1;
		int j = right;
		while (true)
		{
			while (p[++i].position[axis] < key);
			while (p[--j].position[axis] > key && j > left);
			if (i >= j)
				break;
			temp = p[i];
			p[i] = p[j];
			p[j] = temp;
		}
		temp = p[i];
		p[i] = p[right];
		p[right] = temp;
		if (i >= m)
			right = i - 1;
		if (i <= m)
			left = i + 1;
	}
}

void PhotonMap::generKdtree() {
	Photon* temp = new Photon[number+1];
	for (int i = 1; i <= number; i++) {
		temp[i] = tPhoton[i];
	}
	balance(temp, 1, 1, number);
	delete[] temp;
}
void PhotonMap::balance(Photon* p, int i, int s, int e) {
	if (s == e) {
		tPhoton[i] = p[s];
		return;
	}
	int m = getMiddle(s, e);
	int axis = 0;
	if (boxMax.x - boxMin.x > boxMax.y - boxMin.y && boxMax.x - boxMin.x > boxMax.z - boxMin.z) {
		axis = 0;
	}
	else
	{
		if (boxMax.y - boxMin.y > boxMax.z - boxMin.z) {
			axis = 1;
		}
		else
		{
			axis = 2;
		}
	}
	sortMiddle(p, s, e, m, axis);
	tPhoton[i] = p[m];
	tPhoton[i].diffAxis = axis;
	if (s < m) {
		double temp = boxMax[axis];
		boxMax[axis] = tPhoton[i].position[axis];
		balance(p, i * 2, s, m - 1);
		boxMax[axis] = temp;
	}
	if (m < e) {
		double temp = boxMin[axis];
		boxMin[axis] = tPhoton[i].position[axis];
		balance(p, i * 2 + 1, m + 1, e);
		boxMin[axis] = temp;
	}
}

void PhotonMap::getNPhoton(KnnPhoton* np) {
	for (int i = 1; i <= number; i++) {
		Photon* curP = &tPhoton[i];
		curP->dis = glm::length(curP->position - np->posi);
		if (curP->dis < np->dis) {
			np->maxHeap.push(*curP);
			np->found++;
		}
	}
}
void PhotonMap::getNPhoton(KnnPhoton* np, int index) {
	if (index > number) {
		return;
	}
	Photon* curP = &tPhoton[index];
	double dis = 0;
	if (index * 2 <= number) {
		dis = np->posi[curP->diffAxis] - curP->position[curP->diffAxis];
		if (dis < 0) {
			getNPhoton(np, index * 2);
			if (dis * dis < np->dis * np->dis)
				getNPhoton(np, index * 2 + 1);
		}
		else {
			getNPhoton(np, index * 2 + 1);
			if (dis * dis < np->dis * np->dis)
				getNPhoton(np, index * 2);
		}
	}
	curP->dis = glm::length(curP->position - np->posi);
	if (curP->dis < np->dis) {
		np->maxHeap.push(*curP);
		np->found++;
	}
}