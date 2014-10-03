#ifndef SQUARE_H
#define SQUARE_H

class Point {
    public:
        Point()
        {
            m_x = 0;
            m_y = 0;
        }
        Point(int x, int y)
        {
            m_x = x;
            m_y = y;
        }
        ~Point()
        {

        }
        int getX()
        const {
            return m_x;
        }
        int getY()
        const {
            return m_y;
        }
        void setX(int x)
        {
            m_x = x;
        }
        void setY(int y)
        {
            m_y = y;
        }

        bool equals(const Point& a)
        const {
            return (m_x == a.getX() && m_y == a.getY());
        }
        bool less(const Point& a)
        const {
            if (this->equals(a))
                return false;
            int a_x = a.getX();
            int a_y = a.getY();
            if (m_x != a_x)
                return (m_x < a_x);
            return (m_y < a_y) ;
        }
        int findInVector(const std::vector<Point>& list)
        const {
            for (int i = 0; i < list.size(); i++)
                if (equals(list[i]))
                    return i;
            return -1;
        }
    private:
        int m_x;
        int m_y;

};

class Square
{
    public:
        Square()
        {
            m_point = Point();
            m_type = EMPTY;
        }
        Square(int x, int y, int type)
        {
            m_point = Point(x,y);
            m_type = type;
        }
        ~Square()
        {

        }
        int getType()
        {
            return m_type;
        }
        void setType(int type)
        {
            m_type = type;
        }

        Point getPoint()
        {
            return m_point;
        }

    protected:
    private:
        int m_type;
        Point m_point;
};

#endif // SQUARE_H
