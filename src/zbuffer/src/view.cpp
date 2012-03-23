/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-21
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <QHBoxLayout>
#include <QSharedPointer>
#include <QImage>
#include <QEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QTimer>

#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/exception/all.hpp>
#include <boost/timer.hpp>

#include <ans/alpha/pimpl.hpp>
#include <ans/alpha/pimpl_impl.hpp>
#include <ans/alpha/method.hpp>

#include "view.hpp"
#include "core/renderer.hpp"
#include "core/camera.hpp"
#include "core/render_operation.hpp"
#include "qrender_target.hpp"
#include "renderer_functions.hpp"
#include "canvas.hpp"
#include "aint/interact.hpp"
#include "aint/interact_impl.hpp"

namespace
{
    const QSize DEFAULT_IMAGE_SIZE = QSize(800, 600);
    const QRgb DEFAULT_BACKGROUND_COLOR = qRgb(0, 0, 0); // qRgb(200, 200, 200);
}

struct cg::view::data_type
{
    typedef QSharedPointer<QImage> image_ptr;
    typedef aint::orbit_interact<cg::camera> interact;
    typedef boost::shared_ptr<interact> interact_ptr;
    typedef boost::shared_ptr<cg::camera> camera_ptr;

    camera_ptr camera;
    cg::view::renderer_ptr renderer;
    image_ptr image;
    cg::canvas *canvas;
    interact_ptr oi;
    bool updated;

    data_type(cg::view::renderer_ptr renderer) : 
        renderer(renderer),
        image(new QImage(DEFAULT_IMAGE_SIZE, QImage::Format_ARGB32))
    {
        camera = boost::make_shared<cg::camera>();
        oi = boost::make_shared<interact>(camera);
    }
};

namespace
{
    struct view_method : cg::view
    {
        void init_camera()
        {
            BOOST_ASSERT(!data->image.isNull());
            BOOST_ASSERT(data->camera);
            cmlex::vector3 eye(3, 4, 5);
            cmlex::vector3 at(0, 0, 0);
            cmlex::vector3 up(0, 1, 0);
            cg::camera cam;
            data->camera->lookat(eye, at, up);
            update_camera_projection();
        }

        void resize_view(QSize size)
        {
            *data->image = QImage(size, QImage::Format_ARGB32);
            update_camera_projection();
            invalid_canvas();
        }

        void update_camera_projection()
        {
            BOOST_ASSERT(!data->image.isNull());
            BOOST_ASSERT(data->camera);
            data->camera->perspective(
                3.14 / 6,
                data->image->width() * 1.0 / data->image->height(),
                0.1,
                1000
                );
        }

        void init_ui()
        {
            auto lay = new QHBoxLayout(this);
            this->setLayout(lay);
            lay->setMargin(0);

            BOOST_ASSERT(!data->image.isNull());
            auto can = data->canvas = new cg::canvas(this, data->image);
            lay->addWidget(can);
            can->setMouseTracking(true);
            can->installEventFilter(this);
        }

        void invalid_canvas()
        {
            data->updated = false;
        }

        void init_timer()
        {
            data->updated = false;
            QTimer *t = new QTimer(this);
            connect(t, SIGNAL(timeout()), this, SLOT(update_canvas()));
            t->start(0);
        }

        void init()
        {
            init_ui();
            init_camera();
            init_timer();
        }
    };

    ans::alpha::functional::method<view_method> method;
}

cg::view::view(QWidget *parent, renderer_ptr renderer) :
    base_type(parent),
    data(renderer)
{
    method(this)->init();
}

cg::view::~view()
{

}

namespace
{
    struct mouse_move_context
    {
        QMouseEvent &e;
        mouse_move_context(QMouseEvent &e) : e(e) {}
    };

    inline aint::point current_position(const mouse_move_context &c)
    {
        return aint::point(c.e.x(), c.e.y());
    }

    inline bool left_button_down(const mouse_move_context &c)
    {
        auto r = c.e.buttons() & Qt::LeftButton;
        return r;
    }
}

bool cg::view::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseMove)
    {
        if (data->oi->mouse_move(mouse_move_context(*static_cast<QMouseEvent*>(event))))
        {
            method(this)->invalid_canvas();
        }
    }
    return false;
}

QSize cg::view::sizeHint() const
{
    if (auto canvas = data->canvas)
    {
        return canvas->sizeHint();
    }
    BOOST_ASSERT(!data->image.isNull());
    return data->image->size();
}

void cg::view::resizeEvent(QResizeEvent *event)
{
    method(this)->resize_view(event->size());
}

void cg::view::update()
{
    data->updated = false;
    update_canvas();
    base_type::update();
}

/// @todo add time recording here
void cg::view::update_canvas()
{
    if (!data->updated)
    {
        if (auto rd = data->renderer.lock())
        {
            boost::timer tm;
            data->image->fill(DEFAULT_BACKGROUND_COLOR);
            try
            {
                cg::render(
                    *rd,
                    cg::qrender_target(*data->image),
                    *data->camera
                    );
            }
            catch (...)
            {
                std::cerr <<
                    "Render failed.\n" <<
                    boost::current_exception_diagnostic_information();
            }
            *data->image = data->image->mirrored();
            data->canvas->update();
            qDebug() << tm.elapsed();
        }
        data->updated = true;
    }
}

void cg::view::update_camera(const boost::function<void(camera&)> &callback)
{
    callback(*data->camera);
    data->updated = false;
}
