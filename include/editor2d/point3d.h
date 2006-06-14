#ifndef __POINT3D_H__
#define __POINT3D_H__

#include<cstdlib>
#include<cmath>
#include<iostream>

#define COMPARATION_OR      0
#define COMPARATION_AND     1
#define COMPARATION_MODULE  2

using namespace std;

template <class T> class Point3D
{
    public:
        Point3D(T x1 = (T)0, T y1 = (T)0, T z1 = (T)0);
        Point3D(Point3D & some);
        ~Point3D();
        Point3D<T> & operator=(Point3D<T> & some);
        Point3D<T> & operator=(T some);
        bool operator==(Point3D<T> & some);
        bool operator==(T some);
        bool operator!=(Point3D<T> & some);
        bool operator!=(T some);
        bool operator<(Point3D<T> & some);
        bool operator<(T some);
        bool operator<=(Point3D<T> & some);
        bool operator<=(T some);
        bool operator>(Point3D<T> & some);
        bool operator>(T some);
        bool operator>=(Point3D<T> & some);
        bool operator>=(T some);
        bool operator!();
        Point3D<T> & operator+(Point3D<T> & some);
        Point3D<T> & operator+(T some);
        Point3D<T> & operator-(Point3D<T> & some);
        Point3D<T> & operator-(T some);
        Point3D<T> & operator+=(Point3D<T> & some);
        Point3D<T> & operator+=(T some);
        Point3D<T> & operator-=(Point3D<T> & some);
        Point3D<T> & operator-=(T some);
        Point3D<T> & operator*(T some);
        Point3D<T> & operator/(T some);
        Point3D<T> & operator*=(T some);
        Point3D<T> & operator/=(T some);
        Point3D<T> & operator+();
        Point3D<T> & operator-();
        Point3D<T> & Get_Lower_Limited_to(T value);
        Point3D<T> & Get_Lower_Limited_to(T vx,T vy,T vz);
        Point3D<T> & Get_Lower_Limited_to(Point3D<T> & value);
        Point3D<T> & Get_Upper_Limited_to(T value);
        Point3D<T> & Get_Upper_Limited_to(T vx,T vy,T vz);
        Point3D<T> & Get_Upper_Limited_to(Point3D<T> & value);
        Point3D<T> & Get_Limited_to(T min, T max);
        Point3D<T> & Get_Limited_to(T minx,T miny,T minz,T maxx,T maxy,T maxz);
        Point3D<T> & Get_Limited_to(Point3D<T> & min, Point3D<T> & max);
        void Set_Values(T vx,T vy,T vz);
        int Get_Comparation_Mode();
        void Set_Comparation_Mode(int some_mode);
        double Module();
        double Module(Point3D<T> &a);
        double Module(T a, T b, T c);
        string Print();

        T x;
        T y;
        T z;

        private:
        int comparation_mode;
};

template <class T>
Point3D<T>::Point3D(T x1, T y1, T z1)
{
    x=x1;
    y=y1;
    z=z1;
    comparation_mode = COMPARATION_OR;
}

template <class T>
Point3D<T>::Point3D(Point3D<T> & some)
{
    x=(T)0;
    y=(T)0;
    z=(T)0;
    (*this)=some;
}

template <class T>
Point3D<T>::~Point3D()
{
    x=(T)0;
    y=(T)0;
    z=(T)0;
    comparation_mode = COMPARATION_OR;
}

template <class T>
Point3D<T>&
Point3D<T>::operator=(Point3D<T>& some)
{
    if(this == &some) return(*this);

    x = some.x;
    y = some.y;
    z = some.z;
    comparation_mode = some.comparation_mode;

    return(*this);
}

template <class T>
Point3D<T>&
Point3D<T>::operator=(T some)
{
    x = some;
    y = some;
    z = some;
    comparation_mode = COMPARATION_OR;
    return(*this);
}











// //////////////////////////////////////////////////////////////////////// //
//                                COMPARATORS                               //
// //////////////////////////////////////////////////////////////////////// //
template <class T>
bool
Point3D<T>::operator==(Point3D<T> & some)
{
    switch(comparation_mode)
    {
        default:
            comparation_mode = COMPARATION_OR;
        case COMPARATION_OR:
            return (x == some.x || y == some.y || z == some.z);
            break;
        case COMPARATION_AND:
            return (x == some.x && y == some.y && z == some.z);
            break;
        case COMPARATION_MODULE:
            return ((*this).Module() == some.Module());
            break;
    }
}

template <class T>
bool
Point3D<T>::operator==(T some)
{
    switch(comparation_mode)
    {
        default:
            comparation_mode = COMPARATION_OR;
        case COMPARATION_OR:
            return (x == some || y == some || z == some);
            break;
        case COMPARATION_AND:
            return (x == some && y == some && z == some);
            break;
        case COMPARATION_MODULE:
            return (*this.Module() == some);
            break;
    }
}

template <class T>
bool
Point3D<T>::operator!=(Point3D<T>& some)
{
    return (!(*this == some));
}

template <class T>
bool
Point3D<T>::operator!=(T some)
{
    return (!(*this == some));
}

template <class T>
bool
Point3D<T>::operator<(Point3D<T>& some)
{
    switch(comparation_mode)
    {
        default:
            comparation_mode = COMPARATION_OR;
        case COMPARATION_OR:
            return (x < some.x || y < some.y || z < some.z);
            break;
        case COMPARATION_AND:
            return (x < some.x && y < some.y && z < some.z);
            break;
        case COMPARATION_MODULE:
            return (*this.Module() < some);
            break;
    }
}

template <class T>
bool
Point3D<T>::operator<(T some)
{
    Point3D<T> aux(some,some,some);
    switch(comparation_mode)
    {
        default:
            comparation_mode = COMPARATION_OR;
        case COMPARATION_OR:
            return (x < some || y < some || z < some);
            break;
        case COMPARATION_AND:
            return (x < some && y < some && z < some);
            break;
        case COMPARATION_MODULE:
            return ((*this).Module() < aux.Module());
            break;
    }
}

template <class T>
bool
Point3D<T>::operator<=(Point3D<T> & some)
{
    return(*this < some || *this == some);
}

template <class T>
bool
Point3D<T>::operator<=(T some)
{
    return(*this < some || *this == some);
}

template <class T>
bool
Point3D<T>::operator>(Point3D<T> & some)
{
    return(!(*this <= some));
}

template <class T>
bool
Point3D<T>::operator>(T some)
{
    return(!((*this) <= some));
}

template <class T>
bool
Point3D<T>::operator>=(Point3D<T> & some)
{
    return(!(*this < some));
}

template <class T>
bool
Point3D<T>::operator>=(T some)
{
    return(!(*this < some));
}












// //////////////////////////////////////////////////////////////////////// //
//                                ARITHMETICS                               //
// //////////////////////////////////////////////////////////////////////// //
template <class T>
Point3D<T>&
Point3D<T>::operator+(Point3D<T> & some)
{
    Point3D<T>* aux = new Point3D<T>(x + some.x, y + some.y, z + some.z);
    return (*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::operator+(T some)
{
    Point3D<T>* aux = new Point3D<T>(x + some, y + some, z + some);
    return (*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::operator-(Point3D<T> & some)
{
    Point3D<T>* aux = new Point3D<T>(x - some.x, y - some.y, z - some.z);
    return (*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::operator-(T some)
{
    Point3D<T>* aux = new Point3D<T>(x - some, y - some, z - some);
    return (*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::operator+=(Point3D<T> & some)
{
    (*this) = (*this) + some;
    return (*this);
}

template <class T>
Point3D<T>&
Point3D<T>::operator+=(T some)
{
    (*this) = (*this) + some;
    return (*this);
}

template <class T>
Point3D<T>&
Point3D<T>::operator-=(Point3D<T> & some)
{
    (*this) = (*this) - some;
    return (*this);
}

template <class T>
Point3D<T>&
Point3D<T>::operator-=(T some)
{
    (*this) = (*this) - some;
    return (*this);
}

template <class T>
Point3D<T>&
Point3D<T>::operator*(T some)
{
    Point3D<T>* aux = new Point3D<T>(x * some, y * some, z * some);
    return (*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::operator/(T some)
{
    Point3D<T>* aux = new Point3D<T>(x / some, y / some, z / some);
    return (*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::operator*=(T some)
{
    (*this) = (*this) * some;
    return (*this);
}

template <class T>
Point3D<T>&
Point3D<T>::operator/=(T some)
{
    (*this) = (*this) / some;
    return (*this);
}

template <class T>
Point3D<T>&
Point3D<T>::operator+()
{
    Point3D<T>* aux = new Point3D<T>(*this);
    return (*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::operator-()
{
    Point3D<T>* aux = new Point3D<T>(-x, -y, -z);
    return (*aux);
}

template <class T>
bool
Point3D<T>::operator!()
{
    Point3D<T>* aux = new Point3D<T>((T)0,(T)0,(T)0);
    return(*this == aux);
}











// //////////////////////////////////////////////////////////////////////// //
//                                  FILTERS                                 //
// //////////////////////////////////////////////////////////////////////// //
template <class T>
Point3D<T>&
Point3D<T>::Get_Lower_Limited_to(T value)
{
    Point3D<T>* aux = new Point3D<T>;
    (*aux).x = x<value? value : x;
    (*aux).y = y<value? value : y;
    (*aux).z = z<value? value : z;
    return(*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::Get_Lower_Limited_to(T vx,T vy,T vz)
{
    Point3D<T>* aux = new Point3D<T>;
    (*aux).x = x<vx? vx : x;
    (*aux).y = y<vy? vy : y;
    (*aux).z = z<vz? vz : z;
    return(*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::Get_Lower_Limited_to(Point3D<T>& value)
{
    Point3D<T>* aux = new Point3D<T>;
    (*aux).x = x<value.x? value.x : x;
    (*aux).y = y<value.y? value.y : y;
    (*aux).z = z<value.z? value.z : z;
    return(*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::Get_Upper_Limited_to(T value)
{
    Point3D<T>* aux = new Point3D<T>;
    (*aux).x = x>value? value : x;
    (*aux).y = y>value? value : y;
    (*aux).z = z>value? value : z;
    return(*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::Get_Upper_Limited_to(T vx,T vy,T vz)
{
    Point3D<T>* aux = new Point3D<T>;
    (*aux).x = x>vx? vx : x;
    (*aux).y = y>vy? vy : y;
    (*aux).z = z>vz? vz : z;
    return(*aux);
}

template <class T>
Point3D<T>&
Point3D<T>::Get_Upper_Limited_to(Point3D<T>& value)
{
    Point3D<T>* aux = new Point3D<T>;
    (*aux).x = x>value.x? value.x : x;
    (*aux).y = y>value.y? value.y : y;
    (*aux).z = z>value.z? value.z : z;
    return(*aux);
}

template <class T>
Point3D<T> &
Point3D<T>::Get_Limited_to(T min, T max)
{
    Point3D<T>* aux = new Point3D<T>(*this);
    (*aux) = (*aux).Get_Lower_Limited_to(min);
    (*aux) = (*aux).Get_Upper_Limited_to(max);
    return(*aux);
}

template <class T>
Point3D<T> &
Point3D<T>::Get_Limited_to(T minx,T miny,T minz,T maxx,T maxy,T maxz)
{
    Point3D<T>* aux = new Point3D<T>(*this);
    (*aux) = (*aux).Get_Lower_Limited_to(minx,miny,minz);
    (*aux) = (*aux).Get_Upper_Limited_to(maxx,maxy,maxz);
    return(*aux);
}

template <class T>
Point3D<T> &
Point3D<T>::Get_Limited_to(Point3D<T> & min, Point3D<T> & max)
{
    Point3D<T>* aux = new Point3D<T>(*this);
    (*aux) = (*aux).Get_Lower_Limited_to(min);
    (*aux) = (*aux).Get_Upper_Limited_to(max);
    return(*aux);
}

template <class T>
void
Point3D<T>::Set_Values(T vx,T vy,T vz)
{
    x = vx;
    y = vy;
    z = vz;
}
















// //////////////////////////////////////////////////////////////////////// //
//                          Comparation Functions                           //
// //////////////////////////////////////////////////////////////////////// //
template <class T>
int
Point3D<T>::Get_Comparation_Mode()
{
    return comparation_mode;
}

template <class T>
void
Point3D<T>::Set_Comparation_Mode(int some_mode)
{
    comparation_mode = some_mode;
    switch(comparation_mode)
    {
        default:                    comparation_mode = COMPARATION_OR;
        case COMPARATION_OR:        comparation_mode = COMPARATION_OR; break;
        case COMPARATION_AND:       comparation_mode = COMPARATION_AND; break;
        case COMPARATION_MODULE:    comparation_mode = COMPARATION_MODULE; break;
    }
}

template <class T>
double
Point3D<T>::Module()
{
    return(sqrt(x*x + y*y + z*z));
}

template <class T>
double
Point3D<T>::Module(Point3D<T> &a)
{
    return(sqrt(a.x*a.x + a.y*a.y + a.z*a.z));
}

template <class T>
double
Point3D<T>::Module(T a, T b, T c)
{
    return(sqrt(a*a + b*b + c*c));
}

template <class T>
string
Point3D<T>::Print()
{
    char cad[32];
    sprintf(cad,"<%d,%d,%d>",x,y,z);
//    string o;
//    o = "<";
//    o += x;
//    o += ",";
//    o += y;
//    o += ",";
//    o += z;
//    o += ">";
    string s = cad;
    return s;
}

#endif // __POINT3D_H__
