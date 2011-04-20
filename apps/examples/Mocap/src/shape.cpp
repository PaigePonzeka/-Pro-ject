/*
 *  shape.cpp
 *  jestureCap
 *
 *  Created by elizabeth pelka on 4/3/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "shape.h"

const float RWALL = 1024.0f; 
const float LWALL = 0.0f;
const float TWALL = 0.0f;
const float BWALL = 768.0f;
const float CEILING = 10.0f;

Shape::Shape(){}
Shape::Shape(float init_location_x,float init_location_y, float init_height, float init_width, 
			 float init_R, float init_G, float init_B, float init_A)
{
	location_x = init_location_x;
	location_y = init_location_y;
	height = init_height;
	width = init_width;
	shape_color.red = init_R;
	shape_color.green = init_G;
	shape_color.blue = init_B;
	shape_color.alpha = init_A;
	velocity_x = 0;
	velocity_x = 0;
	grabbed = false;
	exploding = false;
	
}

void Shape::move()
{
	location_x += velocity_x;
	location_y += velocity_y;
	if(outsideBounds_x(LWALL,RWALL)){
		printf("BEFORE hitwall vel:%f \n",velocity_x);

		location_x-=(velocity_x);
		velocity_x/= -1;
		printf("hitwall vel:%f \n",velocity_x);
	}
	if(outsideBounds_y(TWALL,BWALL)){
		printf("BEFORE hitwall vel:%f \n",velocity_y);

		location_y-=(velocity_y);
		velocity_y/= -1;
		printf("hitwall vely:%f \n",velocity_y);

	}
	checkCollision(1);
}
void Shape::slow()
{	float area = 0.0f;//getHeight()*getWidth()/1200;
	if(velocity_x!=0){
		if((velocity_x<1)&&(velocity_x>-1)) velocity_x = 0;
		if(velocity_x>0)velocity_x-=area;
		if(velocity_x<0)velocity_x+=area;
	}
	if(velocity_y!=0){
		if((velocity_y<1)&&(velocity_y>-1)) velocity_y = 0;
		if(velocity_y>0)velocity_y-=area;
		if(velocity_y<0)velocity_y+=area;
	}

}
void Shape::velocityCeiling(){
	if(getVelocity_x()>CEILING){
		setVelocity(CEILING, getVelocity_y()-(CEILING - getVelocity_x()));
	}
	if(getVelocity_y()>CEILING){
		setVelocity( getVelocity_x()-(CEILING - getVelocity_y()),CEILING);
	}
}
bool Shape::explosion(){
	return exploding;
}
void Shape::createExplosion(){
	for(int i = 0; i<10 ;i++){
		bubbles[i] = new struct CIRCLE;
	//	bubbles[i].x = ofRandom(getLocation_x(), getLocation_x()+getWidth());
	//	bubbles[i].y = ofRandom(getLocation_y(), getLocation_y()+getHeight());

	}
}
void Shape::collision_AntiMagnet(float x, float y){
	float horz_Middle, vert_Middle;
	horz_Middle = getLocation_y()+(getHeight()/2);
	vert_Middle = getLocation_x()+(getWidth()/2);
	
	setVelocity(x-vert_Middle,y-horz_Middle);
	//*
	if(y>horz_Middle) setVelocity(getVelocity_x(),getVelocity_y()-(getHeight()/2)-1);
	else setVelocity(getVelocity_x(),getVelocity_y()+(getHeight()/2)+1);
	if(x>vert_Middle) setVelocity(getVelocity_x()-(getWidth()/2)-1,getVelocity_y());
	else setVelocity(getVelocity_x()+(getWidth()/2)+1,getVelocity_y());

	if(y>horz_Middle) setVelocity(getVelocity_x(),getVelocity_y()-(getHeight()/2)-1);
	else setVelocity(getVelocity_x(),getVelocity_y()+(getHeight()/2)+1);
	if(x>vert_Middle) setVelocity(getVelocity_x()-(getWidth()/2)-1,getVelocity_y());
	else setVelocity(getVelocity_x()+(getWidth()/2)+1,getVelocity_y());
	//*/
	
	setVelocity(getVelocity_x()/2, getVelocity_y()/2);
	//printf("velocity %f, %f\n",velocity_x,velocity_y);
}
void Shape::collision_Bounce(int i){
	if ((notMoving())&&(Shape::board[i]->notMoving())) collision_AntiMagnet(Shape::board[i]->getLocation_x()+(Shape::board[i]->getWidth()/2), 
																			Shape::board[i]->getLocation_y()+(Shape::board[i]->getHeight()/2));
	else{
		float ux, uy, m2, m1, mass_ratio1, mass_ratio2, mass_fraction1, mass_fraction2;
		ux = Shape::board[i]->getVelocity_x();
		uy = Shape::board[i]->getVelocity_y();
		m2 = Shape::board[i]->getWidth() * Shape::board[i]->getHeight();
		m1 = getWidth()*getHeight();
		mass_ratio1 = (m1-m2)/(m1+m2);
		mass_fraction1 = (2*m2)/(m1+m2);
		mass_ratio2 = (m2-m1)/(m1+m2);
		mass_fraction2 = (2*m1)/(m1+m2);
		setVelocity(getVelocity_x()*mass_ratio1 + ux*mass_fraction1,
					getVelocity_y()*mass_ratio1 + uy*mass_fraction1);
		Shape::board[i]->setVelocity(ux*mass_ratio2 + getVelocity_x()*mass_fraction2,
									 uy*mass_ratio2 + getVelocity_y()*mass_fraction2);
		//printf("Bounce %i [%f %f]\n",i,getVelocity_x(),getVelocity_y());
	}
}	
void Shape::checkCollision(int index)
{	float shapeT,shapeB,shapeL,shapeR;
	
	for(int i=0; i<10; i++){
		//if(Shape::board[i] == this) break;
		shapeL = Shape::board[i]->getLocation_x();
		shapeR = shapeL + Shape::board[i]->getWidth();

		shapeT = Shape::board[i]->getLocation_y();
		shapeB = shapeT + Shape::board[i]->getHeight();
		
		//transfer momentum
		if(insideBounds(shapeL, shapeR, shapeT, shapeB)){
			float xvel = (Shape::board[i]->getVelocity_x()-getVelocity_x())/2;
			float yvel = (Shape::board[i]->getVelocity_y()-getVelocity_y())/2;
			setVelocity(xvel, yvel);
			Shape::board[i]->setVelocity(-1*(xvel), -1*(yvel));
			//collision_Bounce(i);
			//collision_AntiMagnet(Shape::board[i]->getLocation_x()+(Shape::board[i]->getWidth()/2), 
			//					 Shape::board[i]->getLocation_y()+(Shape::board[i]->getHeight()/2));
		}

	}
}
bool Shape::hoveredOver(float x, float y)
{
	float left = location_x;
	float right = location_x + width;
	float top = location_y;
	float bottom = location_y + height;
	
	if((x > left)&&(x < right)&&
	   (y > top)&&(y < bottom)){
		return true;
	}
	else return false;
	
}
bool Shape::notMoving(){
	return ((getVelocity_x()==0)&&(getVelocity_y()==0));
}		   
bool Shape::outsideBounds(float left, float right, float top, float bottom)
{
	if(((location_x < left)||((location_x + width) > right))&&
	   ((location_y < top)||((location_y + height) > bottom))){
		return true;
	}
	else return false;
}
		   
bool Shape::outsideBounds_x(float left,float right)
{
	if((location_x < left)||((location_x + width) > right)){
		return true;
	}
	else return false;
}
bool Shape::outsideBounds_y(float top, float bottom)
{
	if((location_y < top)||((location_y + height) > bottom)){
		return true;
	}
	else return false;
}

bool Shape::insideBounds(float left, float right, float top, float bottom)
{
	if((insideBounds_x(left, right))&&(insideBounds_y(top, bottom))){
		return true;
	}
	else return false;
}
bool Shape::insideBounds_x(float left, float right)
{
	if((location_x > left)&&((location_x) < right)||
	   ((location_x+width) > left)&&((location_x+width) < right)||
	   (location_x < left)&&((location_x+width) > right)||
		(location_x > left)&&((location_x+width) < right))
	{
		return true;
	}
	else return false;
}
bool Shape::insideBounds_y( float top, float bottom)
{
	if((location_y > top)&&((location_y) < bottom)||
	   ((location_y + height)> top)&&((location_y + height) < bottom)||
	   (location_y < top)&&((location_y + height) > bottom)||
	   (location_y > top)&&((location_y + height) < bottom)){
		return true;
	}
	else return false;
}

float Shape::getLocation_x()
{
	return location_x;
}

float Shape::getLocation_y()
{
	return location_y;
}

float Shape::getWidth()
{
	return width;
}

float Shape::getHeight()
{
	return height;
}

float Shape::getRed()
{
	return shape_color.red;
}

float Shape::getGreen()
{
	return shape_color.green;
}

float Shape::getBlue()
{
	return shape_color.blue;
}

float Shape::getAlpha()
{
	return shape_color.alpha;
}

void Shape::setVelocity(float vel_x, float vel_y)
{
	velocity_x = vel_x;
	velocity_y = vel_y;
}
float Shape::getVelocity_x(){
	return velocity_x;
}
float Shape::getVelocity_y(){
	return velocity_y;
}
void Shape::setColor(float R, float G, float B, float A)
{
	shape_color.red = R;
	shape_color.green = G;
	shape_color.blue = B;
	shape_color.alpha = A;
}
void Shape::setLocation(float x, float y)
{
	location_x = x;
	location_y = y;
}

void Shape::grabShape(int hand)
{
	grabbed_by = hand;
	grabbed = true;
}

void Shape::releaseShapeFrom(int hand)
{
	if(grabbed_by == hand){
		grabbed_by = 0;
		grabbed = false;
	}
}
bool Shape::isGrabbed()
{
	return grabbed;
}
bool Shape::isGrabbedBy(int hand)
{	
	return((grabbed_by == hand)&&grabbed);
}