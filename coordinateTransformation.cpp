#include <cmath>

class starCoordinate
{
public:
	starCoordinate();
	starCoordinate(double, double);
	~starCoordinate();
	double getStarX() const { return cos(alpha)*cos(zeta); }
	double getStarY() const { return sin(alpha)*cos(zeta); }
	double getStarZ() const { return sin(zeta); }
private:
	double alpha, zeta;
};

starCoordinate::starCoordinate() : alpha(0),zeta(0)
{
}

starCoordinate::starCoordinate(double inputAlpha, double inputZeta) : alpha(inputAlpha), zeta(inputZeta)
{
}

starCoordinate::~starCoordinate()
{
}

double getNavStarDis(starCoordinate navStarA, starCoordinate navStarB)
{
	return acos(navStarA.getStarX()*navStarB.getStarX() + navStarA.getStarY()*navStarB.getStarY() + navStarA.getStarZ()*navStarB.getStarZ());
}

class cameraCoordinate
{
public:
	cameraCoordinate();
	cameraCoordinate(double, double, double, double);
	~cameraCoordinate();
	double getCameraF() const { return f; }
	double getMatrixA1() const { return sin(alpha)*cos(theta) - cos(alpha)*sin(zeta)*sin(theta); }
	double getMatrixA2() const { return -sin(alpha)*sin(theta) - cos(alpha)*sin(zeta)*cos(theta); }
	double getMatrixA3() const { return -cos(alpha)*cos(zeta); }
	double getMatrixB1() const { return -cos(alpha)*cos(theta) - sin(alpha)*sin(zeta)*sin(theta); }
	double getMatrixB2() const { return cos(alpha)*sin(theta) - sin(alpha)*sin(zeta)*cos(theta); }
	double getMatrixB3() const { return -sin(alpha)*cos(zeta); }
	double getMatrixC1() const { return cos(alpha)*sin(theta); }
	double getMatrixC2() const { return cos(alpha)*cos(theta); }
	double getMatrixC3() const { return -sin(zeta); }
private:
	double alpha, zeta, theta, f;
};

cameraCoordinate::cameraCoordinate() :alpha(0), zeta(0), theta(0), f(0)
{
}

cameraCoordinate::cameraCoordinate(double inputAlpha, double inputZeta, double inputTheta, double inputF) : alpha(inputAlpha), zeta(inputZeta), theta(inputTheta), f(inputF)
{
}

cameraCoordinate::~cameraCoordinate()
{
}



class spotCoordinate
{
public:
	spotCoordinate();
	spotCoordinate(double, double);
	~spotCoordinate();
	double getSpotX() const { return spotX; }
	double getSpotY() const { return spotY; }
private:
	double spotX, spotY;
};

spotCoordinate::spotCoordinate():spotX(0),spotY(0)
{
}

spotCoordinate::spotCoordinate(double inputSpotX, double inputSpotY):spotX(inputSpotX),spotY(inputSpotY)
{
}

spotCoordinate::~spotCoordinate()
{
}

double getSpotDis(spotCoordinate spotA, spotCoordinate spotB, cameraCoordinate cam)
{
	return acos((spotA.getSpotX()*spotB.getSpotX() + spotA.getSpotY()*spotB.getSpotY() + cam.getCameraF()*cam.getCameraF()) / (sqrt(spotA.getSpotX()*spotA.getSpotX() + spotA.getSpotY()*spotA.getSpotY() + cam.getCameraF()*cam.getCameraF())*sqrt(spotB.getSpotX()*spotB.getSpotX() + spotB.getSpotY()*spotB.getSpotY() + cam.getCameraF()*cam.getCameraF())));
}

double starToSpotX(starCoordinate star, cameraCoordinate cam)
{
	return cam.getCameraF()*(cam.getMatrixA1()*star.getStarX() + cam.getMatrixB1()*star.getStarY() + cam.getMatrixC1()*star.getStarZ()) / (cam.getMatrixA3()*star.getStarX() + cam.getMatrixB3()*star.getStarY() + cam.getMatrixC3()*star.getStarZ());
}

double starToSpotY(starCoordinate star, cameraCoordinate cam)
{
	return cam.getCameraF()*(cam.getMatrixA2()*star.getStarX() + cam.getMatrixB2()*star.getStarY() + cam.getMatrixC2()*star.getStarZ()) / (cam.getMatrixA3()*star.getStarX() + cam.getMatrixB3()*star.getStarY() + cam.getMatrixC3()*star.getStarZ());
}