#include <Qt3D/QGLView.h>

class Viewport : public QGLView
{
Q_OBJECT
public:
	Viewport() :
	  QGLView(0)
	{
	}

	void initializeGL(QGLPainter * painter)
    {

	}

	void paintGL(QGLPainter *painter)
	{
	}

private:
};