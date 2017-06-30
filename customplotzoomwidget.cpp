//From http://www.qcustomplot.com/index.php/support/forum/227
#include "customplotzoomwidget.h"
#include "ui_configplot.h"

CustomPlotZoomWidget::CustomPlotZoomWidget(QWidget * parent)
    : QCustomPlot(parent)
    , mZoomMode(false)
    , mRubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));

}

void CustomPlotZoomWidget::OpenConfigPlot() {

    QDialog dlg;
    Ui_configPlotDlg dlg_ui;
    dlg_ui.setupUi(&dlg);

    dlg.exec();
}


void CustomPlotZoomWidget::ShowContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), this);

   QAction action1("Open Config Plot", this);
   connect(&action1, SIGNAL(triggered()), this, SLOT(OpenConfigPlot()));
   contextMenu.addAction(&action1);

   contextMenu.exec(mapToGlobal(pos));

}



CustomPlotZoomWidget::~CustomPlotZoomWidget()
{
    delete mRubberBand;
}

void CustomPlotZoomWidget::setZoomMode(bool mode)
{
    mZoomMode = mode;
}

void CustomPlotZoomWidget::mousePressEvent(QMouseEvent * event)
{
    if (mZoomMode)
    {
        if (event->button() == Qt::RightButton)
        {
            mOrigin = event->pos();
            mRubberBand->setGeometry(QRect(mOrigin, QSize()));
            mRubberBand->show();
        }
    }
    QCustomPlot::mousePressEvent(event);
}

void CustomPlotZoomWidget::mouseMoveEvent(QMouseEvent * event)
{
    if (mRubberBand->isVisible())
    {
        mRubberBand->setGeometry(QRect(mOrigin, event->pos()).normalized());
    }
    QCustomPlot::mouseMoveEvent(event);
}

void CustomPlotZoomWidget::mouseReleaseEvent(QMouseEvent * event)
{
    if (mRubberBand->isVisible())
    {
        const QRect & zoomRect = mRubberBand->geometry();
        int xp1, yp1, xp2, yp2;
        zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
        double x1 = xAxis->pixelToCoord(xp1);
        double x2 = xAxis->pixelToCoord(xp2);
        double y1 = yAxis->pixelToCoord(yp1);
        double y2 = yAxis->pixelToCoord(yp2);


        xAxis->setRange(x1, x2);
        yAxis->setRange(y1, y2);

        mRubberBand->hide();
        replot();
    }
    QCustomPlot::mouseReleaseEvent(event);
}
