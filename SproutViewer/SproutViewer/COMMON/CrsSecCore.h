#pragma once



/* ------------------------------------
class CrsSecCore

date 2016/08/20 
written by Takashi Ijiri

This class manages cross sections to visualize volumes.
-------------------------------------*/




#include "OglForMFC.h"
#include "TMesh.h"


enum CRSSEC_ID
{
	CRSSEC_NON,
	CRSSEC_XY,
	CRSSEC_YZ,
	CRSSEC_ZX,
	CRSSEC_CURVE

};

class CrsSecCore
{
	float m_planeXY; //[0,1]
	float m_planeYZ; //[0,1]
	float m_planeZX; //[0,1]

	EVec3f m_curveCrsSecNorm;
	TMesh  m_curveCrsSec;
	vector<EVec2i> m_curveCrsSecEdge;

	CrsSecCore();
public:
	~CrsSecCore();

	static CrsSecCore *getInst() {
		static CrsSecCore p;
		return &p;
	}


	CRSSEC_ID pickCrosSec(bool bXY, bool bYZ, bool bZX, const EVec3f &cuboid, const EVec3f &rayP, const EVec3f &rayD             );
	CRSSEC_ID pickCrosSec(bool bXY, bool bYZ, bool bZX, const EVec3f &cuboid, const EVec3f &rayP, const EVec3f &rayD, EVec3f &pos);
	
	void drawCrosSec(bool bXY, bool bYZ, bool bZX, const EVec3f &cuboid );
	void moveCrossSec(EVec3i reso, EVec3f pitch, CRSSEC_ID id, short Delta);

	void genCurveCrsSec( const EVec3f &cuboid, const EVec3f &camP, const vector<EVec3f> &stroke);

private:

};





void t_drawAxis();
void t_drawFrame (const EVec3f &cuboid);
void t_drawSlices(const int slineNum, const EVec3f &camP, const EVec3f &camF, const EVec3f &cuboid);

/*
inline  void t_drawCircle
(
	const EVec3f &pos   , //場所
	const int flgPosNeg, //フラグ
	const bool isActive,  //この制御点がアクティブかどうか
	const float radius
)
{
	if( !isActive) return;

	if( flgPosNeg ==1) glColor3d(1,0.5,0.5);
	else if(flgPosNeg ==2) glColor3d(0.5,1.0,0.5);
	else            glColor3d(0.5,0.5,1);
	glLineWidth(10);
	const int N = 100;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < N; ++i)
	{
		double x = pos[0] + radius * cos(i / (double)N * 2 * M_PI);
		double y = pos[1] + radius * sin(i / (double)N * 2 * M_PI);
		glVertex3d(x, y, pos[2]);
	}
	glEnd();
	
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < N; ++i)
	{
		double y = pos[1] + radius * cos(i / (double)N * 2 * M_PI);				
		double z = pos[2] + radius * sin(i / (double)N * 2 * M_PI);
		glVertex3d(pos[0], y, z);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < N; ++i)
	{
			double x = pos[0] + radius * cos(i / (double)N * 2 * M_PI);
			double z = pos[2] + radius * sin(i / (double)N * 2 * M_PI);
			glVertex3d(x, pos[1], z);
	}
	glEnd();
}
*/

inline void t_drawSphere
(
	const EVec3f &pos, //場所
	const float radius
)
{
	const int M = 20;
	const int N = 20;

	glPushMatrix();
	glTranslated(pos[0], pos[1], pos[2]);


	//最下部	

	glBegin(GL_TRIANGLE_FAN);

	glNormal3d(0, 0, 1);
	glVertex3d(0, 0, -radius);

	for (int i = 0; i <= M; i++)
	{
		double theta = i * (M_PI * 2 / M);
		double phi   = M_PI / N - M_PI * 0.5;
		double x = cos(phi) * cos(theta);
		double y = cos(phi) * sin(theta);
		double z = sin(phi);

		glNormal3d(x, y, z);
		glVertex3d(radius * x, radius * y, radius * z);
	}

	glEnd();

	//内部
	for (int j = 1; j < N - 1; j++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i <= M; i++)
		{
			double phi   = j * M_PI / N - M_PI * 0.5;
			double theta = i * M_PI * 2/ M ;
			double x = cos(phi) * cos(theta);
			double y = cos(phi) * sin(theta);
			double z = sin(phi);
			glNormal3d(x, y, z);
			glVertex3d(radius * x, radius * y, radius * z);

			phi = (j + 1) * (M_PI / (double)N) - M_PI * 0.5;
			x = cos(phi) * cos(theta);
			y = cos(phi) * sin(theta);
			z = sin(phi);

			glNormal3d(x, y, z);
			glVertex3d(radius * x, radius * y, radius * z);
		}
		glEnd();
	}

	//最頂部
	glBegin(GL_TRIANGLE_FAN);

	glNormal3d(0, 0, 1);
	glVertex3d(0,0, radius);

	for (int i = 0; i <= M; i++)
	{
		double phi = (N - 1) * M_PI / N - M_PI * 0.5;
		double theta = i * M_PI * 2 / M;
		double x = cos(phi) * cos(theta);
		double y = cos(phi) * sin(theta);
		double z = sin(phi);

		glNormal3d(x, y, z);
		glVertex3d(radius * x, radius * y, radius * z);
	}

	glEnd();

	glPopMatrix();
}



inline void t_drawCylinder
(
	const EVec3f &p1,
	const EVec3f &p2,
	const float  R
)
{
	const int N = 20;

	EVec3f dir = (p2 - p1).normalized();
	EVec3f zAxis(0, 0, 1);
	EVec3f axis = zAxis.cross(dir).normalized();

	float theta = acos( zAxis.dot(dir) );
	Eigen::AngleAxisf Rot(theta, axis);


	vector <EVec3f> v1(N + 1), v2(N + 1), norm(N + 1);
	for (int i = 0; i <= N; i++) 
	{
		float t = i * ((float) M_PI * 2.0f / N);
		float x = cos( t );
		float y = sin( t );
		norm[i] = Rot * EVec3f(x, y, 0);
		v1[i]   = Rot * EVec3f(R *x, R *y, 0) + p1;
		v2[i]   = Rot * EVec3f(R *x, R *y, 0) + p2;
	}

	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i <= N; ++i)
	{
		glNormal3fv( norm[i].data() );
		glVertex3fv(   v1[i].data());
		glVertex3fv(   v2[i].data());
	}
	glEnd();

}


/*
inline void t_drawCylinder
(
	const vector <EVec3f> &pos, //点群
	const int flgPosNeg, //フラグ
	const bool isActive,  //この制御点がアクティブかどうか
	const float radius
)
{
	if (!isActive) return;

	const int N = 20;

	float r = (flgPosNeg == 1) ? 1.0f : 0;
	float b = (flgPosNeg == 1) ? 0 : 1.0f;

	float spec[4] = { 1,1,1,1 };
	float diff[4] = { r,0,b,0 };
	float ambi[4] = { 0.3f*r,0,0.3f*b,1 };
	float shin[1] = { 64.0f };

	const EVec3f z_axis(0,0,1);
	vector <EVec3f> direction;

	for (int i = 0; i < pos.size() -1; i++) {
		direction.push_back((pos[i + 1] - pos[i]).normalized()) ;
	}
	direction.push_back(direction[pos.size() - 2]);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);


	glEnable(GL_LIGHTING);

	for (int i = 0; i < pos.size() - 1 ; i++) {
		glPushMatrix();
		//glTranslated(pos[i][0], pos[i][1], pos[i][2]);

		EVec3f d_avg1, d_avg2;

		if(i != 0)
		{
			d_avg1 = (direction[i - 1] + direction[i]) / 2.0f;
		}
		else //i == 0
		{
			d_avg1 = direction[i];
		}
		d_avg2 = (direction[(i+1) - 1] + direction[(i+1)]) / 2.0f;
		EVec3f axis1 = z_axis.cross(d_avg1).normalized();
		EVec3f axis2 = z_axis.cross(d_avg2).normalized();

		float theta1 = acos(z_axis.dot(d_avg1));
		float theta2 = acos(z_axis.dot(d_avg2));
		Eigen::AngleAxisf R1(theta1, axis1);
		Eigen::AngleAxisf R2(theta2, axis2);

		vector <EVec3f> p1, p2;
		for (int j = 0; j <= N; j++) {
			double phi = j * (M_PI * 2 / N);
			double x = cos(phi);
			double y = sin(phi);
			EVec3f temp((float)x, (float)y,0.0);
			p1.push_back(temp);
			p2.push_back(temp);
			p1[j] = R1*p1[j];
			p2[j] = R2*p2[j];

		}		

		//glRotatef((float)(theta1 * 180.0 / M_PI), axis1[0], axis1[1], axis1[2]);

		glBegin(GL_TRIANGLE_STRIP);

		for (int j = 0; j <= N; j++)
		{
			glNormal3d(p1[j][0], p1[j][1], p1[j][2]);
			p1[j] *= radius;
			p1[j] += pos[i];
			glVertex3d(p1[j][0], p1[j][1], p1[j][2]);

			glNormal3d(p2[j][0], p2[j][1], p2[j][2]);
			p2[j] *= radius;
			p2[j] += pos[(i+1)];
			glVertex3d(p2[j][0], p2[j][1], p2[j][2]);
		}

		glEnd();

		glPopMatrix();

	}

	
	{
		EVec3f a(1, 1, 1), b(1, 2, 3);
		a << 1, 2, 3;

		EVec3f c = a + b;

		float d = a.dot( b );
		EVec3f cro  = a.cross(b).normalized();

		Eigen::AngleAxisf R( (float) M_PI * 0.5f, EVec3f(1, 0, 0) );

		cro = R * cro;

		EMat3f M;
		M(0, 0) = 1;
		M(1, 0) = 0;
		M(2, 0) = 0;

		M(0, 1) = 0;
		M(1, 1) = 1;
		M(2, 1) = 0;
		
		M(0, 2) = 0;
		M(1, 2) = 0;
		M(2, 2) = 1;

		cro = M * cro;

	}
	

	
	glDisable(GL_LIGHTING);
}
*/