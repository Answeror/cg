/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-19
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#undef min
#undef max
#include <algorithm>

#include <boost/make_shared.hpp>
#include <boost/timer.hpp>
#include <boost/assert.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range.hpp>
#include <boost/format.hpp>

#include <QDebug>
#include <QString>
#include <QAction>
#include <QFileDialog>
#include <QLayout>
#include <QPixmap>
#include <QMenuBar>
#include <QMessageBox>
#include <QSharedPointer>
#include <QTimer>
#include <QStatusBar>
#include <QDockWidget>

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/alpha/method.hpp>

#include "mainwindow.hpp"
#include "core/wavefront_obj_loader.hpp"
#include "core/render_model.hpp"
#include "core/renderer.hpp"
#include "core/model.hpp"
#include "view.hpp"
#include "core/log.hpp"
#include "widgets/log_table.hpp"

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

struct cg::mainwindow::data_type
{
    typedef boost::shared_ptr<cg::renderer> renderer_ptr;

    struct actions_t
    {
        QAction *open;
        QAction *rotate;
    } actions;

    struct timer_t
    {
        QTimer *rotate;
    } timers;

    cg::view *view;

    cg::wavefront_obj_loader loader;
    renderer_ptr renderer;

    data_type() :
        renderer(boost::make_shared<cg::renderer>())
    {
    }
};

namespace
{
    struct mainwindow_method : cg::mainwindow
    {
        void init_actions()
        {
            data->actions.open = new QAction(tr("&Open"), this);
            connect(data->actions.open, SIGNAL(triggered()), this, SLOT(open()));
            data->actions.rotate = new QAction(tr("&Rotate"), this);
            data->actions.rotate->setCheckable(true);
            data->actions.rotate->setChecked(false);
            connect(data->actions.rotate, SIGNAL(toggled(bool)), this, SLOT(rotate(bool)));
        }

        void init_menu()
        {
            auto file_menu = menuBar()->addMenu(tr("&File"));
            file_menu->addAction(data->actions.open);
            auto view_menu = menuBar()->addMenu(tr("&View"));
            view_menu->addAction(data->actions.rotate);
        }

        void init_log_table()
        {
            auto dock = new QDockWidget(tr("Log"));
            addDockWidget(Qt::LeftDockWidgetArea, dock);
            dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
            auto log = new cg::widgets::log_table(dock);
            dock->setWidget(log);
        }

        void init_ui()
        {
            init_menu();

            BOOST_ASSERT(data->renderer);
            data->view = new cg::view(this, data->renderer);
            setCentralWidget(data->view);
            //this->layout()->setSizeConstraint(QLayout::SetFixedSize);
            //init_status_bar();
            render_cube();
            init_log_table();
        }

        //void init_status_bar()
        //{
        //    auto tm = new QTimer(this);
        //    connect(tm, SIGNAL(timeout()), this, SLOT(update_status_bar()));
        //    tm->start(0);
        //}

        void init()
        {
            init_actions();
            init_ui();
        }

        void clear_render_pipeline()
        {
            BOOST_ASSERT(data->renderer);
            data->renderer->clear_vertex_buffer();
        }

        void render_cube()
        {
            BOOST_ASSERT(data->renderer);
            clear_render_pipeline();
            data->renderer->be(cg::render_operation::quads(), &cube);
            BOOST_ASSERT(data->view);
            data->view->update();
        }

        void render_none()
        {
            BOOST_ASSERT(data->renderer);
            clear_render_pipeline();
            BOOST_ASSERT(data->view);
            data->view->update();
        }
    };

    ans::alpha::functional::method<mainwindow_method> method;
}

cg::mainwindow::mainwindow() : data(ans::alpha::pimpl::use_default_ctor())
{
    method(this)->init();
}

cg::mainwindow::~mainwindow()
{

}

using cml::x;
using cml::y;
using cml::z;

namespace
{
    /// normalize vertex to [-1.414,1.414]
    void normalize(cg::model &m)
    {
        using boost::for_each;
        cg::vector3 c(0, 0, 0);
        for_each(m.triangles, [&](const cg::model::triangle &t){
            c += t.a.position + t.b.position + t.c.position;
        });
        c /= 3 * boost::size(m.triangles);
        double len = 0;
        for_each(m.triangles, [&](cg::model::triangle &t){
            len = std::max(len, length(t.a.position -= c));
            len = std::max(len, length(t.b.position -= c));
            len = std::max(len, length(t.c.position -= c));
        });
        len /= 1.414;
        for_each(m.triangles, [&](cg::model::triangle &t){
            t.a.position /= len;
            t.b.position /= len;
            t.c.position /= len;
        });
    }
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
        try {
            if (auto model = data->loader.load(path.toStdString())) {
                normalize(*model);
                method(this)->clear_render_pipeline();
                cg::render(*data->renderer, *model);
                BOOST_ASSERT(data->view);
                data->view->update();
                setWindowTitle(path);
            } else {
        		QMessageBox::warning(this, tr("OBJ File Invalid"), 
        			tr("File %1 is not a valid obj model file.").arg(path));
            }
        } catch (...) {
            std::cerr << boost::current_exception_diagnostic_information();
        }
    }
}

QSize cg::mainwindow::sizeHint() const
{
    if (auto view = data->view)
    {
        return view->size();
    }
    return base_type::sizeHint();
}

//void cg::mainwindow::update_status_bar()
//{
//    if (bofu::at_key<tags::render_count>(log))
//    {
//    	statusBar()->showMessage(QString::fromStdString(str(
//            boost::format("average render time: %.3lf, average transform time: %.3lf, planf time: %.3f, make table time: %.3f, update aet time: %.3f")
//    		% (bofu::at_key<tags::render_time>(log) / bofu::at_key<tags::render_count>(log))
//            % (bofu::at_key<tags::transform_time>(log) / bofu::at_key<tags::render_count>(log))
//            % (bofu::at_key<tags::planf_time>(log) / bofu::at_key<tags::render_count>(log))
//            % (bofu::at_key<tags::make_table_time>(log) / bofu::at_key<tags::render_count>(log))
//            % (bofu::at_key<tags::update_aet_time>(log) / bofu::at_key<tags::render_count>(log))
//            )));
//    }
//}

void cg::mainwindow::rotate_one_step_about_y()
{
    data->view->update_camera([](camera &cam){
        yaw_around_world_y(cam, radian(0.1));
    });
}

void cg::mainwindow::rotate(bool yes)
{
    if (yes)
    {
        data->timers.rotate = new QTimer(this);
        connect(data->timers.rotate, SIGNAL(timeout()), this, SLOT(rotate_one_step_about_y()));
        data->timers.rotate->start(100);
    }
    else
    {
        if (data->timers.rotate)
        {
            data->timers.rotate->stop();
        }
    }
}
