#include "rect3i.h"
#include <engine.h>

Vector<Rect3i> difference(Rect3i A, Rect3i B)
{
	Vector3i A_min = A.pos;
	Vector3i B_min = B.pos;
	Vector3i A_max = A.pos + A.size;
	Vector3i B_max = B.pos + B.size;
	// elements of new in the left of old (smaller x)
	Vector3i pos;
	Vector<Rect3i> result;
	if (A_min.x < B_min.x)
	{
		Vector3i A_rect_size = Vector3i(MIN(B_min.x - A_min.x, A.size.x), A.size.y, A.size.z);
		result.push_back(Rect3i(A_min, A_rect_size));
		A_min.x = B_min.x;
		A.pos.x = B.pos.x;
		A.size.x = A_max.x - A_min.x;
	}
	// elements of new under old (smaller y) and not already handled
	if (A_min.y < B_min.y)
	{
		Vector3i A_rect_size = Vector3i(A.size.x, MIN(B_min.y - A_min.y, A.size.y), A.size.z);
		result.push_back(Rect3i(A_min, A_rect_size));
		A_min.y = B_min.y;
		A.pos.y = B.pos.y;
		A.size.y = A_max.y - A_min.y;
	}
	if (A_min.z < B_min.z)
	{
		Vector3i A_rect_size = Vector3i(A.size.x, A.size.y, MIN(B_min.z - A_min.z, A.size.z));
		result.push_back(Rect3i(A_min, A_rect_size));
		A_min.z = B_min.z;
		A.pos.z = B.pos.z;
		A.size.z = A_max.z - A_min.z;
	}

	if (A_max.x > B_max.x)
	{
		Vector3i A_rect_pos = Vector3i(B_max.x, A_min.y, A_min.z);
		Vector3i A_rect_size = Vector3i(MIN(A_max.x - B_max.x, A.size.x), A.size.y, A.size.z);
		result.push_back(Rect3i(A_rect_pos, A_rect_size));
		A_max.x = B_max.x;
		A.size.z = A_max.x - A_min.x;
	}
	// elements of new under old (smaller y) and not already handled
	if (A_max.y > B_max.y)
	{
		Vector3i A_rect_pos = Vector3i(A_min.x, B_max.y, A_min.z);
		Vector3i A_rect_size = Vector3i(A.size.x, MIN(A_max.y - B_max.y, A.size.y), A.size.z);
		result.push_back(Rect3i(A_rect_pos, A_rect_size));
		A_max.y = B_max.y;
		A.size.y = A_max.y - A_min.y;
	}
	if (A_max.z > B_max.z)
	{
		Vector3i A_rect_pos = Vector3i(A_min.x, A_min.y, B_max.z);
		Vector3i A_rect_size = Vector3i(A.size.x, A.size.y, MIN(A_max.z - B_max.z, A.size.z));
		result.push_back(Rect3i(A_rect_pos, A_rect_size));
		A_max.z = B_max.z;
		A.size.z = A_max.z - A_min.z;
	}
	return result;
}
