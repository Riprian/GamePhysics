#include "MassSpringSystemSimulator.h"

MassSpringSystemSimulator::MassSpringSystemSimulator()
{
	m_iTestCase = EULER;
	m_externalForce = Vec3();
	m_inumPoints = m_inumSprings = m_curTime = 0;
	m_fMass = 10.f;
	m_fStiffness = 40.f;
	addMassPoint(Vec3(0, 0, 0), Vec3(-1, 0, 0), FALSE);
	addMassPoint(Vec3(0, 2, 0), Vec3(1, 0, 0), TRUE);
	//the first and second point in m_points
	addSpring(0, 1, 1);
}

const char * MassSpringSystemSimulator::getTestCasesStr()
{
	return "Euler, Leapfrog, Midpoint";
}

void MassSpringSystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;
	switch (m_iTestCase)
	{
	case 0:
		TwAddVarRW(DUC->g_pTweakBar, "# Points", TW_TYPE_INT32, &m_inumPoints, "min=1");
		TwAddVarRW(DUC->g_pTweakBar, "# Springs", TW_TYPE_INT32, &m_inumSprings, "min=1");
		break;
	case 1:break;
	case 2:break;
	default:break;
	}
}

void MassSpringSystemSimulator::reset()
{
	m_mouse.x = m_mouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;

	//reset points' velocity and position
	
}

void MassSpringSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{	
		//MassPoints
		for (int i = 0; i < m_inumPoints; i++) {	
			
			DUC->setUpLighting(Vec3(), Vec3(1, 1, 0), 5.0f, Vec3(1, 0.5f, 0.65f));
			DUC->drawSphere(m_points[i].position, Vec3(0.05, 0.05, 0.05));
		}

		//Springs
		for (int i = 0; i < m_inumSprings; i++) {
			DUC->beginLine();
			DUC->drawLine(m_points[m_springs[i].point1].position, Vec3(0, 1, 0), m_points[m_springs[i].point2].position, Vec3(0, 0, 1));			
			DUC->endLine();
		}
	
}

void MassSpringSystemSimulator::notifyCaseChanged(int testCase)
{
	m_iTestCase = testCase;
	m_curTime = 0;
	//reset position and velocity of point that moves
	//TODO: own method
	m_points[0].position = Vec3();
	m_points[0].velocity = Vec3(-1, 0, 0);
	switch (m_iTestCase)
	{
	case EULER:
		cout << "Euler !\n";	
		break;
	case LEAPFROG:
		cout << "Leapfrog\n";		
		break;
	case MIDPOINT:
		cout << "Midpoint !\n";		
		break;
	default:
		cout << "Empty Test!\n";
		break;
	}
	cout << "position of mass0 at time 0: " << m_points[0].position << "\n";
	cout << "velocity of mass0 at time 0: " << m_points[0].velocity << "\n";
}

void MassSpringSystemSimulator::externalForcesCalculations(float timeElapsed)
{
}

void MassSpringSystemSimulator::simulateTimestep(float timeStep)
{
	timeStep = 0.01f;

	switch (m_iTestCase) {
	case EULER:

		for (int i = 0; i < m_inumPoints; i++) {
			if (!m_points[i].isFixed)
			{
				//iterate velocity and position; acceleration depends on position				
				Vec3 acc = Vec3(-1.f,-1.f,-1.f) * (m_fStiffness / m_fMass) * m_points[i].position;
								
				m_points[i].position = m_points[i].position + timeStep * m_points[i].velocity;

				m_points[i].velocity = m_points[i].velocity + timeStep * acc;					

				if (m_curTime < timeStep) {
					cout << "position of mass1 at first step: " << m_points[i].position << "\n";
					cout << "velocity of mass1 at first step: " << m_points[i].velocity << "\n";
				}
			}
		}

		break;
	case LEAPFROG:

		for (int i = 0; i < m_inumPoints; i++) {
			if (!m_points[i].isFixed)
			{
				Vec3 acc = Vec3(-1.f, -1.f, -1.f) * (m_fStiffness / m_fMass) * m_points[i].position;

				m_points[i].velocity = m_points[i].velocity + timeStep * acc;

				m_points[i].position = m_points[i].position + timeStep * m_points[i].velocity;

				if (m_curTime < timeStep) {
					cout << "position of mass1 at first step: " << m_points[i].position << "\n";
					cout << "velocity of mass1 at first step: " << m_points[i].velocity << "\n";
				}
			}
		}

		break;
	case MIDPOINT:

		for (int i = 0; i < m_inumPoints; i++) {
			if (!m_points[i].isFixed)
			{
				//yTilde
				Vec3 halfPos = m_points[i].position + timeStep / 2 * m_points[i].velocity;
				//acc at yTilde
				Vec3 acc = Vec3(-1.f, -1.f, -1.f) * (m_fStiffness / m_fMass) * halfPos;
				//velocity at yTilde
				m_points[i].velocity = m_points[i].velocity + timeStep * acc;
				//new position
				m_points[i].position = m_points[i].position + timeStep * m_points[i].velocity;

				if (m_curTime < timeStep) {
					cout << "position of mass1 at first step: " << m_points[i].position << "\n";
					cout << "velocity of mass1 at first step: " << m_points[i].velocity << "\n";
				}
			}
		}
		break;
	default:
		break;
	}

	m_curTime += timeStep;
	
}

void MassSpringSystemSimulator::onClick(int x, int y)
{
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

void MassSpringSystemSimulator::onMouse(int x, int y)
{
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

void MassSpringSystemSimulator::setMass(float mass)
{
	m_fMass = mass;
}

void MassSpringSystemSimulator::setStiffness(float stiffness)
{
	m_fStiffness = stiffness;
}

void MassSpringSystemSimulator::setDampingFactor(float damping)
{
	m_fDamping = damping;
}

int MassSpringSystemSimulator::addMassPoint(Vec3 position, Vec3 Velocity, bool isFixed)
{
	MassPoint point;
	point.position = position;
	point.velocity = Velocity;
	point.isFixed = isFixed;
	m_points.push_back(point);
	m_inumPoints++;
	return m_inumPoints - 1;
}

void MassSpringSystemSimulator::addSpring(int masspoint1, int masspoint2, float initialLength)
{
	Spring spring;
	spring.point1 = masspoint1;
	spring.point2 = masspoint2;
	spring.initialLength = initialLength;
	m_springs.push_back(spring);
	m_inumSprings++;	
}

int MassSpringSystemSimulator::getNumberOfMassPoints()
{
	return m_inumPoints;
}

int MassSpringSystemSimulator::getNumberOfSprings()
{
	return m_inumSprings;
}

Vec3 MassSpringSystemSimulator::getPositionOfMassPoint(int index)
{
	return m_points[index].position;
}

Vec3 MassSpringSystemSimulator::getVelocityOfMassPoint(int index)
{
	return m_points[index].velocity;
}

void MassSpringSystemSimulator::applyExternalForce(Vec3 force)
{
}
