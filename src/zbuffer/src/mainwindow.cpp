/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <QString>
#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QPixmap>
#include <QMenuBar>
#include <QMessageBox>

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/alpha/method.hpp>

#include "mainwindow.hpp"
#include "core/wavefront_obj_loader.hpp"
#include "core/render_model.hpp"
#include "core/renderer.hpp"
#include "core/model.hpp"
#include "core/camera.hpp"
#include "qrender_target.hpp"
#include "renderer_functions.hpp"

namespace
{
    void cube(cg::renderer::primitive &p)
    {
    	//    v6----- v5
    	//   /|      /|
    	//  v1------v0|
    	//  | |     | |
    	//  | |v7---|-|v4
    	//  |/      |/
    	//  v2------v3

    	// face v0-v1-v2-v3
    	p.normal3d(0,0,1);
    	p.color3f(1,1,1);
    	p.vertex3d(1,1,1);
    	p.color3f(1,1,0);
    	p.vertex3d(-1,1,1);
    	p.color3f(1,0,0);
    	p.vertex3d(-1,-1,1);
    	p.color3f(1,0,1);
    	p.vertex3d(1,-1,1);

    	// face v0-v3-v4-v6
    	p.normal3d(1,0,0);
    	p.color3f(1,1,1);
    	p.vertex3d(1,1,1);
    	p.color3f(1,0,1);
    	p.vertex3d(1,-1,1);
    	p.color3f(0,0,1);
    	p.vertex3d(1,-1,-1);
    	p.color3f(0,1,1);
    	p.vertex3d(1,1,-1);

    	// face v0-v5-v6-v1
    	p.normal3d(0,1,0);
    	p.color3f(1,1,1);
    	p.vertex3d(1,1,1);
    	p.color3f(0,1,1);
    	p.vertex3d(1,1,-1);
    	p.color3f(0,1,0);
    	p.vertex3d(-1,1,-1);
    	p.color3f(1,1,0);
    	p.vertex3d(-1,1,1);

    	// face  v1-v6-v7-v2
    	p.normal3d(-1,0,0);
    	p.color3f(1,1,0);
    	p.vertex3d(-1,1,1);
    	p.color3f(0,1,0);
    	p.vertex3d(-1,1,-1);
    	p.color3f(0,0,0);
    	p.vertex3d(-1,-1,-1);
    	p.color3f(1,0,0);
    	p.vertex3d(-1,-1,1);

    	// face v7-v4-v3-v2
    	p.normal3d(0,-1,0);
    	p.color3f(0,0,0);
    	p.vertex3d(-1,-1,-1);
    	p.color3f(0,0,1);
    	p.vertex3d(1,-1,-1);
    	p.color3f(1,0,1);
    	p.vertex3d(1,-1,1);
    	p.color3f(1,0,0);
    	p.vertex3d(-1,-1,1);

    	// face v4-v7-v6-v5
    	p.normal3d(0,0,-1);
    	p.color3f(0,0,1);
    	p.vertex3d(1,-1,-1);
    	p.color3f(0,0,0);
    	p.vertex3d(-1,-1,-1);
    	p.color3f(0,1,0);
    	p.vertex3d(-1,1,-1);
    	p.color3f(0,1,1);
    	p.vertex3d(1,1,-1);
    }
}

namespace cg
{
    struct mainwindow::data_type
    {
        struct actions_t
        {
            QAction *open;
        } actions;

        QLabel *canvas;
        QImage target;

        wavefront_obj_loader loader;
        renderer rd;
        camera cam;

        data_type() : target(800, 600, QImage::Format_ARGB32) {}
    };

    struct mainwindow_method : mainwindow
    {
        void init_actions()
        {
            data->actions.open = new QAction(tr("&Open"), this);
            connect(data->actions.open, SIGNAL(triggered()), this, SLOT(open()));
        }

        void init_menu()
        {
            auto filemenu = menuBar()->addMenu(tr("&File"));
            filemenu->addAction(data->actions.open);
        }

        void init_ui()
        {
            init_menu();

            data->canvas = new QLabel(this);
            setCentralWidget(data->canvas);
            this->layout()->setSizeConstraint(QLayout::SetFixedSize);
        }

        void init_camera()
        {
            cmlex::vector3 eye(3, 4, 5);
            cmlex::vector3 at(0, 0, 0);
            cmlex::vector3 up(0, 1, 0);
            cg::camera cam;
            data->cam.lookat(eye, at, up);
            data->cam.perspective(
                3.14 / 6,
                data->target.width() * 1.0 / data->target.height(),
                0.1,
                1000
                );
        }

        void update()
        {
            data->target.fill(qRgb(200, 200, 200));
            cg::render(data->rd, cg::qrender_target(data->target), data->cam);
            data->canvas->setPixmap(QPixmap::fromImage(data->target.mirrored()));
        }
        
        void render_cube()
        {
            data->rd.be(cg::render_operation::quads(), &cube);
            update();
        }

        void init()
        {
            init_actions();
            init_ui();
            init_camera();
            render_cube();
        }
    };

    namespace
    {
        ans::alpha::functional::method<mainwindow_method> method;
    }
}

cg::mainwindow::mainwindow() : data(ans::alpha::pimpl::use_default_ctor())
{
    method(this)->init();
}

cg::mainwindow::~mainwindow()
{

}

void cg::mainwindow::open()
{
    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Open wavefront obj model"),
        "objs",
        tr("Obj files (*.obj)")
        );
    if (!path.isEmpty())
    {
        if (auto model = data->loader.load(path.toStdString())) {
            cg::render(data->rd, *model);
            method(this)->update();
            setWindowTitle(path);
        } else {
    		QMessageBox::warning(this, tr("OBJ File Invalid"), 
    			tr("File %1 is not a valid obj model file.").arg(path));
        }
    }
}

QSize cg::mainwindow::sizeHint() const
{
    return data->target.size();
}
