#pragma once
// Miscellaneous helper functions
#define ROUND(x)				((x - floor(x) > 0.5f) ? ceil(x) : floor(x))
#define SAFE_DELETE(p)			{if (p) {delete p; p = 0;}}
#define SAFE_DELETE_ARRAY(p)	{if (p) {delete [] p; p = 0;}}
#define SAFE_RELEASE(p)			{if (p) {p->Release(); p = 0;}}
// bounds
struct BOUNDS
{
	int						nX, nY; 
	int						xMin, xMax, yMin, yMax;			
	inline bool ptBelong(int initX, int initY)
	{		
		return (initX >= xMin) && (initX <= xMax) && (initY >= yMin) && (initY <= yMax);	
	}
	inline bool ptBelong1(int initX, int initY)
	{		
		return (initX >= 0) && (initX <= xLen()) && (initY >= 0) && (initY <= yLen());	
	}
	inline int xLen()
	{
		return xMax - xMin;
	};
	inline int yLen()
	{
		return yMax - yMin;
	}
	inline int square()
	{
		return nX * nY;
	}	
};
// limits
struct LIMITS : public BOUNDS
{			
	float zMin, zMax;	
	inline LIMITS()
	{
		zMin = zMax = -1.0f;
	}
	inline float zRange()
	{
		return zMax - zMin;
	}	
};
// geometry
struct GEOMETRY : public BOUNDS
{	
	int						xStep, yStep;		
	inline void getxMax()
	{
		xMax = xMin + xStep*(nX - 1);
	};	
	inline void getyMax()
	{
		yMax = yMin + yStep*(nY - 1);
	};
}; 
// space
struct SPACE : public GEOMETRY
{
	int						nZ;
	inline int cube()
	{
		return square() * nZ;
	}
};
// param
struct PARAM
{
	double M, S, K, Cl;
};